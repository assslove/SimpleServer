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

#include <sys/epoll.h>
#include <errno.h>
#include <malloc.h>

#include "global.h"
#include "master.h"


int master_init()
{
	epinfo.epfd = epoll_create(setting.nr_max_event);
	if (epinfo.epfd == -1) {
		ERROR(0, "create epfd error: %s", strerror(errno));
		return -1;
	}

	epinfo.evs = (struct epoll_event *)calloc(setting.nr_max_event, sizeof(struct epoll_event));		
	if (epinfo.evs == -1) {
		ERROR(0, "create epoll events error: %s", strerror(errno));
		return -1;
	}

	epinfo.fds = (fd_wrap_t *)calloc(setting.nr_max_fd, sizeof(fd_wrap_t));		
	if (epinfo.fds == -1) {
		ERROR(0, "create epoll fds error: %s", strerror(errno));
		return -1;
	}

	return 0;
}

int master_listen(int i)
{
	work_t *works = workmgr.works;
	int listenfd = safe_socket_listen(works[i].ip, works[i].port SOCK_STREAM, 1024, 1024);
	if (listenfd == -1) {
		ERROR(0, "listen error[i][%s]", i, strerror(errno));
		return -1;
	}

	int ret = add_fd_to_epinfo(epinfo.fd, listenfd, EPOLLIN);
	if (ret == -1) {
		ERROR(0, "add fd to epinfo error [%s]", strerror(errno));
		return -1;
	}
	
	return 0;
}

int master_dispatch()
{
	while (!stop) {
		int i = 0;
		int fd = 0, newfd = 0;
		struct sockaddr_in cliaddr;
		int clilen = sizeof(cliaddr);
		int ret = 0;
		int n = epoll_wait(epinfo.epfd, epinfo.evs, epinfo.nr_max_event, 10);
		for (i = 0; i < n; i++) {
			fd = epinfo.evs[i].data.fd;
			if (epinfo.evs[i].events && EPOLLIN) { // read
				if (fd == fd_listen_type) { //listen
					newfd = safe_tcp_accept(fd, &cliaddr, 1);	
					if (newfd == -1) {
						ERROR(0, "accept error: [%s]", strerror(errno));
							continue;
					}

					if ((ret = add_fd_to_epinfo(epinfo.epfd, newfd, EPOLLIN)) == -1) {
						ERROR(0, "add fd to epinfo error[fd=%d][%s]", newfd, strerror(errno));
						return -1;
					}
					save_fd(newfd, id, fd_type_, &cliaddr);
				} else if (fd == fd_type_) { //read

				}
			} else if (epinfo.evs[i].events && EPOLLOUT) { //write
				
			}
		}
	}

	return 0;	
}

int master_fini()
{
	return 0;	
}
