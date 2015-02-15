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
 *         Author:	houbin , houbin-12@163.com
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

	//msg_queue init
	msg_queue_t *msgq = &epinfo.msgq;
	if ((ret = mq_init(&(msgq->rq), setting.mem_queue_len, MEM_TYPE_RECV)) == -1) {
		ERROR(0, "init rq fail");
		return -1;
	}
	//发送队列
	if ((ret = mq_init(&(msg->sq), setting.mem_queue_len, MEM_TYPE_SEND)) == -1) {
		ERROR(0, "init wq fail");
		return -1;
	}

	//初始化发送队列通知管道
	pipe(msgq->send_pipefd);
	if ((ret = add_fdinfo_to_epinfo(msgq->send_pipefd[0], i, fd_type_pipe, 0, 0)) == -1) {  //用于接收子进程的读取
		return -1;
	} 

	//初始化变量信息
	workmgr.nr_used = setting.worknum;
	return 0;
}

int master_listen()
{
	int listenfd = safe_socket_listen(setting->bind_ip, setting->bind_port, SOCK_STREAM, 1024, setting.raw_buf_len);
	if (listenfd == -1) {
		ERROR(0, "listen error[i][%s]", i, strerror(errno));
		return -1;
	}

	int ret = 0;
	if ((ret = add_fdinfo_to_epinfo(listenfd, i, fd_type_listen, inet_addr(setting.bind_ip), setting->bind_port)) == -1) {
		return -1;
	}
	//close pipe
	close(epinfo->msgq.sq.pipefd[1]); //主进程发送管道关闭写 等待子进程写
	int i = 0;
	for (; i < setting.worknum; ++i) {
		work_t *work = &workmgr.works[i];
		close(work->recv_pipefd[0]); //接收管道关闭读 主要用于写，通知子进程
	}

	if (so.serv_init && so.serv_init(1)) { //主进程初始化
		ERROR(0, "parent serv init failed");
		return -1;
	}

	INFO(0, "master have listened [%s:%d]", setting.bind_ip, setting.bind_port);

	return 0;
}


int master_recv_pipe_create(int i) 
{
	work_t *work = &workmgr.works[i];
	work->id = i; //编号从0开始
	pipe(work->recv_pipefd);
	return 0;
}

int master_dispatch()
{
	while (!stop) {
		//handle closelist
		handle_closelist(1);
		//handle readlist
		handle_readlist(1);
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
				ERROR(0, "master wait failed fd=%d", fd);
				continue;
			}

			if (epinfo.evs[i].events & EPOLLIN) { // read
				switch (epinfo.fds[fd].type) {
					case fd_type_listen: //监听 一直打开
						while (do_fd_open(fd) != -1) ;
						break;
					case fd_type_cli: //read cli;
						if (handle_cli(fd) == -1) {
							do_fd_close(fd, 1);
						}
						break; 
					case fd_type_pipe: //read pipe;
						handle_pipe(fd);
						break;
				}
			} else if (epinfo.evs[i].events & EPOLLOUT) { //write
				if (epinfo.fds[fd].buff.slen > 0) {
					if (do_fd_write(fd) == -1) {
						do_fd_close(fd, 1);
					}
				}

				if (epinfo.fds[fd].buff.slen == 0) { //发送完毕
					mod_fd_to_epinfo(epinfo.epfd, fd, EPOLLIN);
				}
			} else if (epinfo.evs[i].events & EPOLLHUP) {
				handle_hup(fd);
			} else if (epinfo.evs[i].events & EPOLLRDHUP) { //测试用
				INFO(0, "fd other peer closed [fd=%d]", fd);
			} else {
				INFO(0, "wait info [fd=%d,events=%d]", fd, epinfo.evs[i].events);
			}
		}
	}

	return 0;
}

int master_fini()
{
	if (so.serv_fini && so.serv_fini(1)) { //主进程清理资源
		ERROR(0, "child serv fini failed");
	}

	mq_fini(&(epinfo.msgq->rq), setting.mem_queue_len);
	mq_fini(&(epinfo.msgq->sq), setting.mem_queue_len);
	close(epinfo.msgq.send_pipefd[0]);

	int i = 0;
	for (; i < workmgr.nr_used; ++i) { //关闭管道
		work_t *work = &workmgr.works[i];
		close(work->recv_pipefd[1]);
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


int handle_cli(int fd)
{
	fd_buff_t *buff = &epinfo.fds[fd].buff;

	if (handle_read(fd) == -1) {
		return -1;
	}

	char *tmp_ptr = buff->rbf;
push_again:
	if (buff->msglen == 0 && buff->rlen > 0) { //获取长度
		buff->msglen = so.get_msg_len(fd, tmp_ptr, buff->rlen, SERV_MASTER);
		TRACE(0, "recv [fd=%u][rlen=%u][msglen=%u]", fd, buff->rlen, buff->msglen);
	}

	//push
	if (buff->rlen >= buff->msglen) {
		mem_block_t blk;		
		raw2blk(fd, &blk);
		if (mq_push(&workmgr.works[epinfo.fds[fd].idx].rq, &blk, tmp_ptr)) { //push error if close cli fd return -1 or 0
			ERROR(0, "mq is full, push failed [fd=%d]", fd);
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
	mem_block_t *tmpblk = NULL;
	while ((tmpblk = mq_pop(&epinfo.msgq.sq)) != NULL) { //获取块
		do_blk_send(tmpblk); //发送块
	}
}

int do_blk_send(mem_block_t *blk)
{
	if (blk->type == BLK_CLOSE) { //如果是待关闭的块 这个是由子进程通知的
		//do_add_to_closelist(blk->fd); //增加进去 有可能重复发给子进程，但是没有关系的
		do_fd_close(blk->fd, 2);
		return 0;
	}
	//合法性校验
	return do_fd_send(blk->fd, blk->data, blk->len - blk_head_len);	
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
			do_fd_close(fd, 2); //不需要通知客户端 
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
	
	const char *srv_name = conf_get_str("srv_name");
	if (srv_name != NULL) {
		memcpy(setting.srv_name, srv_name, sizeof(setting.srv_name));
	}
	setting.raw_buf_len = conf_get_int("raw_buf_len");

	const char *data_so = conf_get_str("data_so");
	setting.data_so = NULL;
	if (data_so != NULL) {
		memcpy(setting.data_so, data_so, sizeof(setting.data_so));
	}

	const char *text_so = conf_get_str("text_so");
	if (text_so == NULL) {
		fprintf(stderr, "load text_so error\n");
		return -1;
	}
	memcpy(setting.text_so, text_so, sizeof(setting.text_so));

	setting.worknum = conf_get_int("work_num");
	setting.log_level = conf_get_int("log_level");
	setting.log_maxfiles = conf_get_int("log_maxfiles");
	setting.log_dir = conf_get_int("log_dir");
	setting.log_size = conf_get_int("log_size");

	const char* bind_ip = conf_get_str("bind_ip");
	if (bind_ip == NULL) {
		fprintf(stderr, "load bind ip failed \n");
		return -1;
	}
	memcpy(setting.bind_ip, bind_ip, sizeof(setting.bind_ip));

	setting.bind_port = conf_get_int("bind_port");
	const char* recv_semname = conf_get_str("recv_semname");
	if (recv_semname == NULL) {
		fprintf(stderr, "recv sem read error\n");
		return -1;
	}
	memcpy(setting.recv_semname, recv_semname, sizeof(setting,recv_semname));

	const char* send_semname = conf_get_str("send_semname");
	if (send_semname == NULL) {
		fprintf(stderr, "send sem read error\n");
		return -1;
	}
	memcpy(setting.send_semname, send_semname, sizeof(setting,send_semname));

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

void  handle_epipe(int signo)
{
	ERROR(0, "recv SIGPIPE");
}

int handle_signal()
{
	//设置子进程结束信号
	struct sigaction sa;
	sa.sa_flags = SA_RESTART | SA_SIGINFO;	
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigchld;
	sigaction(SIGCHLD, &sa, NULL);

	sa.sa_handler = handle_epipe;
	sigaction(SIGPIPE, &sa, NULL);

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
