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

	int ret = add_fd_to_epinfo(epinfo.epfd, listenfd, EPOLLIN);
	if (ret == -1) {
		ERROR(0, "add fd to epinfo error [%s]", strerror(errno));
		return -1;
	}
	//save fd
	struct in_addr addr;
	inet_aton(work->ip, &addr);
	//save_fd(listenfd, i, fd_type_listen, addr.s_addr, work->port);
	
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
		//handle closelist
		
		int i = 0;
		int fd = 0, newfd = 0;
		struct sockaddr_in cliaddr;
		int ret = 0;
		int n = epoll_wait(epinfo.epfd, epinfo.evs, setting.nr_max_event, 10);
		for (i = 0; i < n; i++) {
			fd = epinfo.evs[i].data.fd;
			if (epinfo.evs[i].events && EPOLLIN) { // read
				if (epinfo.fds[fd].type == fd_type_listen) { //listen
					newfd = safe_tcp_accept(fd, &cliaddr, 1);	
					if (newfd == -1) {
						ERROR(0, "accept error: [%s]", strerror(errno));
						continue;
					}

					if ((ret = add_fd_to_epinfo(epinfo.epfd, newfd, EPOLLIN)) == -1) {
						ERROR(0, "add fd to epinfo error[fd=%d][%s]", newfd, strerror(errno));
						return -1;
					}
				//	save_fd(newfd, epinfo.fds[fd].id, fd_type_cli, cliaddr.sin_addr.s_addr, cliaddr.sin_port);
				} else if (epinfo.fds[fd].type == fd_type_cli) { //read

				}
			} else if (epinfo.evs[i].events && EPOLLOUT) { //write
				
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

int add_fdinfo_to_epinfo(int fd, uint8_t type, int id, int ip, uint16_t port, int events)
{
	set_io_nonblock(fd, 1);

	struct epoll_event event;
	event.data.fd = fd;
	event.events = events | EPOLLET;

	int ret = epoll_ctl(epinfo.epfd, EPOLL_CTL_ADD, fd, &event);	
	if (ret == -1) {
		ERROR(0, "err ctl add [%s]", strerror(errno));
		return -1;
	}

	fd_wrap_t *pfd = &epinfo.fds[fd];
	++epinfo.seq;
	pfd->type = type;
	pfd->id = id;
	pfd->fd = fd;
	pfd->addr.ip = ip;
	pfd->addr.port = port;

	if (epinfo.max_fd < fd) epinfo.max_fd = fd;

	return 0;
}

