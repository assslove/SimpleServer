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
#include <sys/epoll.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/mman.h>

#include <glib.h>

#include "global.h"
#include "master.h"
#include "log.h"
#include "list.h"
#include "mem_queue.h"
#include "net_util.h"
#include "util.h"

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
	INIT_LIST_HEAD(&readlist);				
	INIT_LIST_HEAD(&closelist);				

	return 0;
}

int master_listen(int i)
{
	work_t *work = &workmgr.works[i];
	int listenfd = safe_socket_listen(work->ip, work->port, SOCK_STREAM, 1024, 1024);
	if (listenfd == -1) {
		ERROR(0, "listen error[i][%s]", i, strerror(errno));
		return -1;
	}

	int ret = 0;
	if ((ret = add_fdinfo_to_epinfo(listenfd, i, fd_type_listen, inet_addr(work->ip), work->port)) == -1) {
		return -1;
	}
	//mem_queue init
	if ((ret = mq_init(&(work->rq), setting.mem_queue_len)) == -1) {
		ERROR(0, "init rq fail");
		return -1;
	}

	if ((ret = mq_init(&(work->wq), setting.mem_queue_len)) == -1) {
		ERROR(0, "init wq fail");
		return -1;
	}

	//close pipe
	close(work->rq.pipefd[1]); //接收管道关闭写
	close(work->wq.pipefd[0]); //发送管理关闭读

	INFO(0, "serv [%d] have listened", i);
	return 0;
}

int master_dispatch()
{
	while (!stop) {
		//handle readlist
		handle_readlist();
		//handle closelist
		handle_closelist();
		//处理发送队列
		handle_mq_send();

		int i = 0;
		int fd = 0, newfd = 0;
		struct sockaddr_in cliaddr;
		int ret = 0;
		int n = epoll_wait(epinfo.epfd, epinfo.evs, setting.nr_max_event, 10);
		for (i = 0; i < n; i++) {
			fd = epinfo.evs[i].data.fd;
			if (epinfo.evs[i].events && EPOLLIN) { // read
				switch (epinfo.fds[fd].type) {
					case fd_type_listen:
						//if (handle_accept(fd, &cli_addr) == -1) {
						//continue;
						//}
						newfd = safe_tcp_accept(fd, &cliaddr, 1);	
						if (newfd == -1) {
							ERROR(0, "accept error: [%s]", strerror(errno));
							continue;
						}

						if ((ret = add_fdinfo_to_epinfo(newfd, epinfo.fds[fd].idx, fd_type_cli, \
										cliaddr.sin_addr.s_addr, cliaddr.sin_port)) == -1) {
							return -1;
						}
						break;
					case fd_type_cli: //read cli;
						handle_cli(fd);
						break; 
					case fd_type_pipe: //read pipe;
						handle_pipe(fd);
						break;
				}
			} else if (epinfo.evs[i].events && EPOLLOUT) { //write
				if (epinfo.fds[fd].buff.sbf > 0) {
					if (do_fd_write(fd) == -1) {
						//do_fd_del(fd);
					}
				}

				if (epinfo.fds[fd].buff.slen == 0) { //发送完毕
					mod_fd_to_epinfo(epinfo.epfd, fd, EPOLLIN);
					
				}
			}
		}
	}

	return 0;
}

int master_fini()
{
	int i = 0;
	for (; i < workmgr.nr_used; i++) {
		work_t *work = &workmgr.works[i];
		mq_fini(&work->rq, setting.mem_queue_len);
		mq_fini(&work->wq, setting.mem_queue_len);
	}

	close(epinfo.epfd);
	free(epinfo.evs);

	DEBUG(0, "serv have stopped!");
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
		ERROR(0, "err ctl add [%s]", strerror(errno));
		return -1;
	}

	fd_wrap_t *pfd = &epinfo.fds[fd];
	++epinfo.seq;
	pfd->type = type;
	pfd->idx = idx;
	pfd->fd = fd;
	pfd->addr.ip = ip;
	pfd->addr.port = port;

	if (epinfo.max_fd < fd) epinfo.max_fd = fd;

	return 0;
}

int handle_cli(int fd)
{
	fd_buff_t *buff = &epinfo.fds[fd].buff;
	uint8_t *tmp_ptr = buff->rbf;

	if (handle_read(fd) == -1) {
		return -1;
	}

push_agagin:
	if (buff->msglen == 0) { //获取长度
		buff->msglen = so.get_msg_len(fd, tmp_ptr, buff->rlen, SERV_MASTER);
		TRACE(0, "recv [fd=%u][rlen=%u][msglen=%u]", fd, buff->rlen, buff->msglen);
	}

	//push
	if (buff->rlen >= buff->msglen) {
		struct mem_block_t blk;		
		raw2blk(fd, &blk);
		if (mq_push(workmgr.works[epinfo.fds[fd].idx].rq, &blk, tmp_ptr)) {
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

	if (buff->rbuf != tmp_ptr && buff->rlen > 0) {  //还有剩余的在缓存区，不够一个消息，等读完了再去push
		memmove(buff->rbuf, tmp_ptr, buff->rlen); //合并到缓冲区头
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
		ERROR(0, "[fd=%u,ip=%s] has closed", fd, inet_ntoa(*((struct in_addr_t *)&epinfo.fds[fd].addr.ip)));
		return -1;
	} else { //
		ERROR(0, "recv error[fd=%u,error=%u]", fd, strerror(errno));
		recv_len = 0;
	}

	if (buff->rbuf == setting.max_msg_len) {
		//增加到可读队列里面
		do_add_to_readlist(fd);	
	} else if {
		//从可读队列里面删除
		do_del_from_readlist(fd);	
	}

	return recv_len;
}

int handle_pipe(fd)
{
	//申请空间 TODO
	if (!epinfo.fds[fd].buff->rbf) {
		epinfo.fds[fd].buff->msglen = 0;
		epinfo.fds[fd].buff->rlen = 0;
		epinfo.fds[fd].buff->rbf = mmap(0, setting.max_msg_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
		if (epinfo.fds[fd].buff->rbf == MAP_FAILED) {
			ERROR(0, "mmap error");
			return -1;
		}
	
	}
	read(fd, epinfo.fds[fd].buff->rbf, setting.max_msg_len);
	return 0;
}


void handle_mq_send()
{
	struct mem_queue_t *tmpq;	
	struct mem_block_t *tmpblk;

	int i = 0;
	for ( ; i < workmgr.nr_used; ++i) {
		tmpq = &workmgr.works[i].sendq;
		while (tmpblk = mq_get(tmpq)) { //获取消息
			do_blk_send(tmpblk);
			mq_pop(tmpq); //删除	
		}
	}
}

int do_blk_send(mem_block_t *blk)
{
	//合法性校验
	return do_fd_send(blk->fd, blk->data, blk->len - setting.blk_len);	
}

int do_fd_send(int fd, void *data, int len)
{
	fd_buff_t *buff = &epinfo.fds[fd].buff;	
	int is_send = 0;
	if (buff->slen > 0) {
		if (do_fd_write(fd) == -1) { //fd断了
			//do_fd_del(fd);
			return -1;	
		}
		is_send = 1;
	}
	
	int send_len = 0;
	if (buff->slen == 0) { //发送缓冲区没有数据发送
		send_len = safe_tcp_send_n(fd, data, len);
		if (send_len == -1) {
			ERROR(0, "write fd error[fd=%u, err=%s]", fd, strerror(errno));
			//do_fd_del(fd);
			return -1;
		} 
	}

	if (len > send_len) { //如果没有发送完
		int left_len = len - send_len;
		if (!buff->sbf) { //没有空间
			buff->sbf = (char *)malloc(left_len);
			if (buff->sbf) {
				ERROR(0, "malloc err, [err=%s]", strerror(errno));
				return -1;
			}
			buff->sbf_size = left_len;
		} else if (buff->sbf_size < buff->slen + left_len) {
			buff->sbf = (char *)realloc(buff->rbf, buff->len + left_len);
			if (!buff->sbf) {
				ERROR(0, "realloc err, [err=%s]", strerror(errno));
				return -1;
			}
			buff->sbf_size = left_len + buff->slen;
		}

		memcpy(buff->sbf + buff->slen, (char *)data + send_len, left_len);
		buff->slen += left_len;

		if (setting.max_sbf_len && setting.max_sbf_len < buff->slen) { //如果大于最大发送缓冲区
			ERROR(0, "sendbuf is exceeded max[fd=%u, buflen=%u,slen=%u]", fd, setting.max_sbf_len, buff->slen);
			//do_fd_del(fd);
			return -1;

		}
	}

	if (buff->slen > 0 && !is_send) { //没有写完 上次没有写过 如果写过 不修改原来事件
		mod_fd_to_epinfo(epinfo.epfd, fd, EPOLLIN | EPOLLOUT);	//当缓冲区不满时继续写
	} else if (buff->slen == 0) { //写完 修改为可读事件
		mod_fd_to_epinfo(epinfo.epfd, fd, EPOLLIN);	
	}

	return 0;
}

int do_fd_write(int fd)
{
	int send_len;
	fd_buff_t *buff = &epinfo.fds[fd].sbf;
	send_len = safe_tcp_send_n(fd, buff->sbf, buff->slen);

	if (send_len == 0) {
		return 0;
	} else if (send_len > 0) {
		if (send_len < buff->slen) {
			memmove(buff->sbf, buff->sbf + send_len, buff->slen - send_len);
			buff->slen -= send_len;
		}
	} else {
		ERROR(0, "write fd error[fd=%u, err=%s]", fd, strerror(errno));
	}

	return send_len;
}

int handle_readlist()
{
	fd_wrap_t *pfd, *tmpfd;
	list_for_each_entry_safe(pfd, tmpfd, &readlist, node) {
		DEBUG(0, "%s [fd=%u]", __func__, pfd->fd);
		if (pfd->type == fd_type_listen) { //打开连接
			
		} else if (handle_cli(pfd->fd) == -1) { //处理客户端的请求 读取
			do_add_to_readlist(pfd->fd);
		}
		list_del(&pfd->readlist);
	}

	return 0;
}

int handle_closelist()
{
	fd_wrap_t *pfd, *tmpfd;
	list_for_each_entry_safe(pfd, tmpfd, &readlist, node) {
		DEBUG(0, "%s [fd=%u]", __func__, pfd->fd);
		if (pfd->buff.slen > 0) { //不再接收
			//写入缓存区
		}

		do_fd_close(fd);

		list_del(&pfd->readlist);
	}

	return 0;
}


int do_add_to_readlist(int fd) 
{
	if (!epinfo.fds[fd].flag) {
		list_add_tail(&epinfo.fds[fd].readlist, &epinfo.node);
		epinfo.fds[fd].flag |= CACHE_READ;
		TRACE(0, "add to readlist [fd=%u]", fd);
	}
}

void do_del_from_readlist(int fd)
{
	if (epinfo.fds[fd].flag & CACHE_READ) {
		epinfo.fds[fd].flag = 0;
		list_del_init(&epinfo.fds[fd].node);
		TRACE(0, "del from readlist [fd=%u]", fd);
	}
}

int do_add_to_closelist(int fd) 
{
	do_del_from_readlist(fd);
	if (!(epinfo.fds[fd].flag & CACHE_CLOSE)) {
		list_add_tail(&epinfo.fds[fd].closelist, &epinfo.node);
		epinfo.fds[fd].flag |= CACHE_CLOSE;
		TRACE(0, "add to closelist[fd=%u]", fd);
	}
}

void do_del_from_closelist(int fd)
{
	if (epinfo.fds[fd].flag & CACHE_CLOSE) {
		epinfo.fds[fd].flag = 0;
		list_del_init(&epinfo.fds[fd].node);
		TRACE(0, "del from closelist[fd=%u]", fd);
	}
}

int do_fd_close(int fd)
{
	if (epinfo.fds[fd].type == fd_type_null) {
		return 0;
	}

	mem_block_t blk;
	blk.id = epinfo.fds[fd].id;
	blk.fd = fd;
	blk.type = BLK_CLOSE;
	blk.len = blk_head_len;

	mq_push(workmgr.works[blk.id].recvq, &blk, NULL);

	//从可读队列中删除
	do_del_from_readlist(fd);
	//从待关闭队列中删除
	do_del_from_closelist(fd);

	//释放缓冲区
	free_buff(&epinfo.fds[fd].buff);
	epinfo.fds[fd].type = fd_type_null;

	close(fd);
	--epinfo.count;

	//替换最大fd
	if (epinfo.maxfd == fd) {
		int i; 
		for (i = fd - 1; i >= 0; --i) {
			if (epinfo.fds[i].type == fd_type_null) {
				break;
			}
		}
		epinfo.maxfd = i;
	}

	INFO(0, "close [fd=%d]", fd);
}


int do_fd_open(int fd) 
{
	return 0;
}
