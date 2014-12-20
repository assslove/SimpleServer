/*
 * =====================================================================================
 *
 *       Filename:  work.c
 *
 *    Description:  子进程处理函数放在此处
 *
 *        Version:  1.0
 *        Created:  12/11/2014 02:00:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <errno.h>
#include <sys/epoll.h>

#include "net_util.h"
#include "util.h"
#include "global.h"
#include "work.h"
#include "log.h"

int work_init(int i)
{
	work_t *work = &workmgr.works[i];
	//chg title
	chg_proc_title("SimpleServer-%d", work->id);
	//release master resource
	close(epinfo.epfd);
	free(epinfo.evs);
	
	epinfo.epfd = epoll_create(setting.nr_max_event);
	if (epinfo.epfd == -1) {
		ERROR(0, "create epfd error: %s", strerror(errno));
		return -1;
	}

	epinfo.evs = (struct epoll_event *)calloc(setting.nr_max_event / 10, sizeof(struct epoll_event));		
	if (epinfo.evs == NULL) {
		ERROR(0, "create epoll events error: %s", strerror(errno));
		return -1;
	}

	epinfo.fds = (fd_wrap_t *)calloc(setting.nr_max_fd, sizeof(fd_wrap_t));		
	if (epinfo.fds == NULL) {
		ERROR(0, "create epoll fds error: %s", strerror(errno));
		return -1;
	}
	
	//close mem_queue pipe

	int k = 0;
	for (; k < workmgr.nr_used; k++) {
		if (k != i) {
			close(work->rq.pipefd[1]);
			close(work->wq.pipefd[0]);
		}
	}

	stop = 0;

	INFO(0, "child serv[%d] have started", i);
	return 0;
}

int work_dispatch(int i)
{
	stop = 0;
	while (!stop) {
		int k = 0;
		int fd = 0;
		int nr = epoll_wait(epinfo.epfd, epinfo.evs, setting.nr_max_event, 100);
		for (k = 0; k < nr; ++k) {
			fd = epinfo.evs[k].data.fd;
			switch (epinfo.fds[fd].type) {
				case fd_type_cli:
					break;
				case fd_type_pipe:
					break;
				case fd_type_svr:
					break;
				case fd_type_mcast:
					break;
			}
		}

		//handle memqueue read
		handle_mq_recv();	
		//handle timer callback
		if (so.handle_timer) {
			so.handle_timer();
		}
	}

	return 0;	
}

int work_fini(int i)
{
	work_t *work = &workmgr.works[i];
	mq_fini(&work->rq, setting.mem_queue_len);
	mq_fini(&work->wq, setting.mem_queue_len);

	close(epinfo.epfd);
	free(epinfo.evs);

	DEBUG(0, "child serv[i] have stopped!", i);

	return 0;	
}

int handle_mq_recv(int i)
{
	struct mem_queue_t *tmpq;	
	struct mem_block_t *tmpblk;

	tmpq = &workmgr.works[i].recvq;
	while (tmpblk = mq_get(tmpq)) {
		do_blk_exec(tmpblk);
		mq_pop(tmpq);
	}
	return 0;
}
