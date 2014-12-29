/*
 * =====================================================================================
 *
 *       Filename:  master.c
 *
 *    Description:  主进程函数放在此处
 *
 *        Version:  1.0
 *        Created:  12/11/2014 01:59:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>

#include <glib.h>

#include "global.h"
#include "master.h"
#include "log.h"
#include "list.h"
#include "mem_queue.h"
#include "net_util.h"
#include "util.h"
#include "outer.h"
#include "conf.h"
#include "work.h"


int master_init()
{
	epinfo.epfd = epoll_create(setting.nr_max_event);
	if (epinfo.epfd == -1) {
		ERROR(0, "create epfd error: %s", strerror(errno));
		return -1;
	}

	epinfo.evs = (struct epoll_event *)calloc(setting.nr_max_event, sizeof(struct epoll_event));		
	if (epinfo.evs == NULL) {
		ERROR(0, "create epoll events error: %s", strerror(errno));
		return -1;
	}

	epinfo.fds = (fd_wrap_t *)calloc(setting.nr_max_fd, sizeof(fd_wrap_t));		
	if (epinfo.fds == NULL) {
		ERROR(0, "create epoll fds error: %s", strerror(errno));
		return -1;
	}

	stop = 0;
	//init list_head
	INIT_LIST_HEAD(&epinfo.readlist);				
	INIT_LIST_HEAD(&epinfo.closelist);				
	
	//load so
	int ret = reg_so(setting.text_so, 0);
	if (ret == -1) {
		return -1;
	}

	reg_data_so(setting.data_so);

	//初始化mem
	return 0;
}

int master_listen(int i)
{
	work_t *work = &workmgr.works[i];
	int listenfd = safe_socket_listen(work->ip, work->port, SOCK_STREAM, 1024, setting.raw_buf_len);
	if (listenfd == -1) {
		ERROR(0, "listen error[i][%s]", i, strerror(errno));
		return -1;
	}

	int ret = 0;
	if ((ret = add_fdinfo_to_epinfo(listenfd, i, fd_type_listen, inet_addr(work->ip), work->port)) == -1) {
		return -1;
	}
	//close pipe
	close(work->rq.pipefd[0]); //接收管道关闭读
	close(work->sq.pipefd[1]); //发送管理关闭写

	INFO(0, "serv [%d] have listened", i);

	return 0;
}


int master_mq_create(int i) 
{
	work_t *work = &workmgr.works[i];
	int ret;
	//mem_queue init
	if ((ret = mq_init(&(work->rq), setting.mem_queue_len)) == -1) {
		ERROR(0, "init rq fail");
		return -1;
	}

	if ((ret = mq_init(&(work->sq), setting.mem_queue_len)) == -1) {
		ERROR(0, "init wq fail");
		return -1;
	}

	if ((ret = add_fdinfo_to_epinfo(work->rq.pipefd[1], i, fd_type_pipe, 0, 0)) == -1) {
		return -1;
	}

	if ((ret = add_fdinfo_to_epinfo(work->sq.pipefd[0], i, fd_type_pipe, 0, 0)) == -1) { 
		return -1;
	} 
	return 0;
}

int master_dispatch()
{
	while (!stop) {
		//handle closelist
		handle_closelist();
		//handle readlist
		handle_readlist();
		//处理发送队列
		handle_mq_send();

		int i, fd;
		int n = epoll_wait(epinfo.epfd, epinfo.evs, setting.nr_max_event, 10);

		if (n == -1 && errno != EINTR) {
			ERROR(0, "master epoll wait error[err=%s]", strerror(errno));
			return -1;
		}

		//刷新时间
		for (i = 0; i < n; i++) {
			fd = epinfo.evs[i].data.fd;
			//判断异常状态
			if (fd > epinfo.maxfd || epinfo.fds[fd].fd != fd) {
				ERROR(0, "error wait fd=%d", fd);
				continue;
			}

			if (epinfo.evs[i].events & EPOLLIN) { // read
				switch (epinfo.fds[fd].type) {
					case fd_type_listen: //监听 一直打开
						while (do_fd_open(fd) != -1) ;
						break;
					case fd_type_cli: //read cli;
						handle_cli(fd);
						break; 
					case fd_type_pipe: //read pipe;
						handle_pipe(fd);
						break;
				}
			} else if (epinfo.evs[i].events & EPOLLOUT) { //write
				if (epinfo.fds[fd].buff.slen > 0) {
					if (do_fd_write(fd) == -1) {
						do_fd_close(fd);
					}
				}

				if (epinfo.fds[fd].buff.slen == 0) { //发送完毕
					mod_fd_to_epinfo(epinfo.epfd, fd, EPOLLIN);
				}
			} else if (epinfo.evs[i].events & EPOLLHUP) {
				handle_hup(fd);
			}
		}
	}

	return 0;
}

int master_fini()
{
	int i = 0;
	for (; i < workmgr.nr_used; ++i) {
		work_t *work = &workmgr.works[i];
		mq_fini(&work->rq, setting.mem_queue_len);
		mq_fini(&work->sq, setting.mem_queue_len);
	}

	for (i = 0; i < epinfo.maxfd; ++i) {
		if (epinfo.fds[i].fd == 0) continue;
		fd_buff_t *buff = &epinfo.fds[i].buff;
		if (buff->rbf) free(buff->rbf);
		if (buff->sbf) free(buff->sbf);
	}

	free(epinfo.evs);
	free(epinfo.fds);
	close(epinfo.epfd);

	DEBUG(0, "master serv have stopped!");

	return 0;	
}

int add_fdinfo_to_epinfo(int fd, int idx, int type, int ip, uint16_t port)
{
	set_io_nonblock(fd, 1);

	struct epoll_event event;
	event.data.fd = fd;
	switch (type) {
		case fd_type_listen:
		case fd_type_pipe:
		case fd_type_cli:
			event.events = EPOLLIN | EPOLLET;
			break;

	}

	int ret = epoll_ctl(epinfo.epfd, EPOLL_CTL_ADD, fd, &event);	
	if (ret == -1) {
		ERROR(0, "err ctl add [fd=%u][%s]", fd, strerror(errno));
		return -1;
	}

	fd_wrap_t *pfd = &epinfo.fds[fd];
	++epinfo.seq;
	pfd->type = type;
	pfd->idx = idx;
	pfd->fd = fd;
	pfd->addr.ip = ip;
	pfd->addr.port = port;

	if (epinfo.maxfd < fd) epinfo.maxfd = fd;

	return 0;
}

int handle_cli(int fd)
{
	fd_buff_t *buff = &epinfo.fds[fd].buff;

	if (handle_read(fd) == -1) {
		return -1;
	}

	char *tmp_ptr = buff->rbf;
push_again:
	if (buff->msglen == 0) { //获取长度
		buff->msglen = so.get_msg_len(fd, tmp_ptr, buff->rlen, SERV_MASTER);
		TRACE(0, "recv [fd=%u][rlen=%u][msglen=%u]", fd, buff->rlen, buff->msglen);
	}

	//push
	if (buff->rlen >= buff->msglen) {
		mem_block_t blk;		
		raw2blk(fd, &blk);
		if (mq_push(&workmgr.works[epinfo.fds[fd].idx].rq, &blk, tmp_ptr)) {
			return -1;
		}
		//清空
		tmp_ptr += buff->msglen;
		buff->rlen -= buff->msglen;
		buff->msglen = 0;

		if (buff->rlen > 0) {//如果没有push完
			goto push_again;
		}
	}

	if (buff->rbf != tmp_ptr && buff->rlen > 0) {  //还有剩余的在缓存区，不够一个消息，等读完了再去push
		memmove(buff->rbf, tmp_ptr, buff->rlen); //合并到缓冲区头
	}

	return 0;
}

int handle_read(int fd)
{
	fd_buff_t *buff = &epinfo.fds[fd].buff;
	if (!buff->rbf) {
		buff->msglen = 0;
		buff->rlen = 0;
		buff->rbf = mmap(0, setting.max_msg_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
		if (buff->rbf == MAP_FAILED) {
			ERROR(0, "mmap error");
			return -1;
		}
	}

	//判断缓存区是否已满
	if (setting.max_msg_len == buff->rlen) {
		ERROR(0, "recv buff full [fd=%u]", fd);
		return 0;
	}

	//接收消息
	int recv_len = safe_tcp_recv_n(fd, buff->rbf + buff->rlen, setting.max_msg_len - buff->rlen);

	if (recv_len > 0) { //有消息
		buff->rlen += recv_len;
	} else if (recv_len == 0) { //对端关闭
		ERROR(0, "[fd=%u,ip=%s] has closed", fd, inet_ntoa(*((struct in_addr *)&epinfo.fds[fd].addr.ip)));
		return -1;
	} else { //
		ERROR(0, "recv error[fd=%u,error=%u]", fd, strerror(errno));
		recv_len = 0;
	}

	if (buff->rlen == setting.max_msg_len) {
		//增加到可读队列里面
		do_add_to_readlist(fd);	
	} else {
		//从可读队列里面删除
		do_del_from_readlist(fd);	
	}

	return recv_len;
}

int handle_pipe(fd)
{
	//申请空间 TODO
	if (!epinfo.fds[fd].buff.rbf) {
		epinfo.fds[fd].buff.msglen = 0;
		epinfo.fds[fd].buff.rlen = 0;
		epinfo.fds[fd].buff.rbf = mmap(0, setting.max_msg_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
		if (epinfo.fds[fd].buff.rbf == MAP_FAILED) {
			ERROR(0, "mmap error");
			return -1;
		}
	
	}
	read(fd, epinfo.fds[fd].buff.rbf, setting.max_msg_len);
	return 0;
}


void handle_mq_send()
{
	mem_queue_t *tmpq;	
	mem_block_t *tmpblk;

	int i = 0;
	for ( ; i < workmgr.nr_used; ++i) {
		tmpq = &workmgr.works[i].sq;
		while ((tmpblk = mq_get(tmpq)) != NULL) { //获取消息
			do_blk_send(tmpblk);
			mq_pop(tmpq); //删除	
		}
	}
}

int do_blk_send(mem_block_t *blk)
{
	if (blk->type == BLK_CLOSE) { //如果是待关闭的块 这个是由子进程通知的
		do_add_to_closelist(blk->fd); //增加进去 有可能重复发给子进程，但是没有关系的
		return 0;
	}
	//合法性校验
	return do_fd_send(blk->fd, blk->data, blk->len - blk_head_len);	
}

int handle_readlist()
{
	fd_wrap_t *pfd, *tmpfd;
	list_for_each_entry_safe(pfd, tmpfd, &epinfo.readlist, node) {
		DEBUG(0, "%s [fd=%u]", __func__, pfd->fd);
		if (pfd->type == fd_type_listen) { //打开连接
			while(do_fd_open(pfd->fd)) {}	 //应该不会执行
		} else if (handle_cli(pfd->fd) == -1) { //处理客户端的请求 读取
			do_fd_close(pfd->fd); //接收失败 关闭
		}
	}

	return 0;
}

int handle_closelist()
{
	fd_wrap_t *pfd, *tmpfd;
	list_for_each_entry_safe(pfd, tmpfd, &epinfo.closelist, node) {
		DEBUG(0, "%s [fd=%u]", __func__, pfd->fd);
		if (pfd->buff.slen > 0) {	//不再接收
			do_fd_write(pfd->fd);		//写入缓存区
		}
		do_fd_close(pfd->fd);
	}

	return 0;
}



int do_fd_open(int fd) 
{
	struct sockaddr_in cliaddr;
	int ret;
	int newfd = safe_tcp_accept(fd, &cliaddr, 1);	
	if (newfd > 0) {
		if ((ret = add_fdinfo_to_epinfo(newfd, epinfo.fds[fd].idx, fd_type_cli, \
						cliaddr.sin_addr.s_addr, cliaddr.sin_port)) == -1) {
			return 0;
		}

		mem_block_t blk;
		blk.id = epinfo.fds[newfd].idx;
		blk.fd = newfd;
		blk.type = BLK_OPEN;
		blk.len = blk_head_len + sizeof(fd_addr_t);

		if ((mq_push(&workmgr.works[blk.id].rq, &blk, &epinfo.fds[newfd].addr)) == -1) {
			do_fd_close(fd); //不需要通知客户端
		}
	}  

	return newfd;
}

int init_setting()
{
	setting.nr_max_event = conf_get_int("nr_max_event");
	setting.nr_max_fd = conf_get_int("nr_max_fd");
	setting.mem_queue_len = conf_get_int("mem_queue_len");
	setting.max_msg_len = conf_get_int("max_msg_len");
	setting.max_buf_len = conf_get_int("max_buf_len");
	
	char *srv_name = conf_get_str("srv_name");
	if (srv_name != NULL) {
		memcpy(setting.srv_name, srv_name, sizeof(setting.srv_name));
	}
	setting.raw_buf_len = conf_get_int("raw_buf_len");

	char *data_so = conf_get_str("data_so");
	if (data_so != NULL) {
		memcpy(setting.data_so, data_so, sizeof(setting.data_so));
	}

	char *text_so = conf_get_str("text_so");
	if (text_so == NULL) {
		ERROR(0, "load text_so error");
		return -1;
	}

	memcpy(setting.text_so, text_so, sizeof(setting.text_so));
	return 0;
}


int master_init_for_work(int id) 
{
	int ret;
	ret = mq_init(&workmgr.works[id].sq, setting.mem_queue_len);
	if (ret == -1) {
		ERROR(0, "map sendq failed[id=%d,err=%s]", id, strerror(errno));
		return -1;
	}

	ret = mq_init(&workmgr.works[id].rq, setting.mem_queue_len);
	if (ret == -1) {
		ERROR(0, "map recvq failed[id=%d,err=%s]", id, strerror(errno));
		return -1;
	}

	if ((ret = add_fdinfo_to_epinfo(workmgr.works[id].sq.pipefd[0], id, fd_type_pipe, 0, 0)) == -1) { //发送队列关闭写管道
		return -1;
	} 

	if ((ret = add_fdinfo_to_epinfo(workmgr.works[id].rq.pipefd[1], id, fd_type_pipe, 0, 0)) == -1) { //接收队列关闭读管道
		return -1;
	}

	//close unused pipefd
	close(workmgr.works[id].sq.pipefd[1]);
	close(workmgr.works[id].rq.pipefd[0]);

	return 0;
}

void raw2blk(int fd, mem_block_t *blk)
{
	blk->id = epinfo.fds[fd].idx;
	blk->len = epinfo.fds[fd].buff.msglen + blk_head_len;
	blk->fd = fd;
	blk->type = BLK_DATA;
}

void handle_sigchld(int signo) 
{
	int pid, status;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) { //回收僵尸进程, 如果没有hang 立即返回
		ERROR(0, "%d have stop", pid);
		int i;
		for (i = 0; i < workmgr.nr_used; i++) { //清零
			if (chl_pids[i] == pid) {
				chl_pids[i] = 0;
				break;
			}
		}
	}
}

/* @brief 结束主进程
 */
void handle_term(int signo)
{
	switch (signo) {
		case SIGQUIT:
			INFO(0, "receive SIGQUIT");
			break;
		case SIGTERM:
			INFO(0, "receive SIGTEAM");
			break;
		case SIGINT:
			INFO(0, "receive SIGINT");
			break;
	}

	int ppid = getpid();
	int isparent = 1;
	int i;	
	for (i = 0; i < workmgr.nr_used; i++) {
		if (chl_pids[i] == ppid) {
			isparent = 0;
			break;
		}
	}
	
	if (isparent) {
		stop = 1; //终止主进程
		for (i = 0; i < workmgr.nr_used; i++) { //终止子进程
			if (chl_pids[i]) {
				chl_pids[i] = 0;
				kill(chl_pids[i], signo);
			}
		}
	}
}

void handle_hup(int fd)
{
	//相应管道被关闭
	int idx = epinfo.fds[fd].idx;
	ERROR(0, "fd have closed [fd=%d,servid=%d]", fd, workmgr.works[idx].id);

	//释放资源
	work_t *work = &workmgr.works[idx];
	mq_fini(&work->rq, setting.mem_queue_len);
	mq_fini(&work->sq, setting.mem_queue_len);

	//创建共享内存
	int ret = master_mq_create(idx);
	if (ret == -1) {
		ERROR(0, "err create mq");
		return ;
	}
	//重启子进程
	int pid = fork();
	if (pid < 0) { //
		ERROR(0, "serv [%d] restart failed", work->id);
		return ;
	} else if (pid == 0) { //child
		int ret = work_init(idx);
		if (ret == -1) {
			ERROR(0, "err work init [%s]", strerror(errno));
			exit(0);
		}
		work_dispatch(idx);
		work_fini(idx);
		exit(0);
	}

	chl_pids[idx] = pid;

	INFO(0, "serv [%d] restart success", work->id);
}


int handle_signal()
{
	//设置子进程结束信号
	struct sigaction sa;
	sa.sa_flags = SA_RESTART | SA_SIGINFO;	
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigchld;
	sigaction(SIGCHLD, &sa, NULL);

	//处理其他异常信号
	sa.sa_handler = handle_term;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	//删除某些信号 这部分需要了解 TODO	
	sigset_t sset;
	sigemptyset(&sset);
	sigaddset(&sset, SIGSEGV);
	sigaddset(&sset, SIGBUS);
	sigaddset(&sset, SIGABRT);
	sigaddset(&sset, SIGILL);
	sigaddset(&sset, SIGCHLD);
	sigaddset(&sset, SIGFPE);
	sigprocmask(SIG_UNBLOCK, &sset, &sset); //删除

	return 0;
}
