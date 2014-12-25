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
#include "outer.h"
#include "fds.h"

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
		if (k == i) {
			close(work->rq.pipefd[1]);
			close(work->wq.pipefd[0]);
		} else { //其它都关闭
			close(workmgr.works[k].rq.pipefd[0]);
			close(workmgr.works[k].rq.pipefd[1]);
			close(workmgr.works[k].sq.pipefd[0]);
			close(workmgr.works[k].sq.pipefd[1]);
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
		int nr = epoll_wait(epinfo.epfd, epinfo.evs, setting.nr_max_event, 10);
		for (k = 0; k < nr; ++k) {
			fd = epinfo.evs[k].data.fd;
			if (epinfo.evs[k].events & EPOLLIN) {
				switch (epinfo.fds[fd].type) {
					case fd_type_pipe:
						do_proc_pipe(fd);
						break;
					case fd_type_svr:
						do_proc_svr(fd);
						break;
					case fd_type_mcast:
						do_proc_mcast(fd);
						break;
					default:
						break;
				}
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
	struct mem_queue_t *recvq = &workmgr.works[i].rq;
	struct mem_queue_t *sendq = &workmgr.works[i].sq;

	struct mem_block_t *tmpblk;

	while (tmpblk = mq_get(recvq)) {
		switch (tmpblk->type) {
			case BLK_DATA: //对客户端消息处理
				do_blk_msg(tmpblk);
				break;
			case BLK_OPEN:
				if (do_blk_open(tmpblk) == -1) { //处理块打开，如果打不到，则关闭
					tmpblk->type = BLK_CLOSE;	
					tmpblk->len = sizeof(mem_block_t);
					mq_push(sendq, tmpblk, NULL);
				}
				break;
			case BLK_CLOSE:
				do_blk_close(recvq->fd); //处理关闭
				break;
		}
		mq_pop(tmpq);
	}
	return 0;
}

int do_blk_msg(mem_block_t *blk)
{
	if (blk->len <= blk_len) {
		ERROR(0, "err blk len[total_len=%u,blk_len=%u]", blk->len, blk_len);
		return 0;
	}

	fdsess_t *fdsess = get_fd(blk->fd);
	if (fdsess) {
		if (so.proc_msg_from_cli(blk->data, blk->len - blk_len, fdsess)) {
			close_cli(blk->fd); //断开连接
		}
	}
	return 0;
}

int do_blk_open(mem_block_t *blk)
{
	fdsess_t *fdsess = get_fd(blk->fd);
	if (fdsess || (blk->len != blk_head_len + 6)) {
		ERROR(0, "fd open error[fd=%u,len=%u]", blk->fd, blk->len);
		return -1;
	} else {
		fdsess_t *sess = g_slice_alloc(sizeof(fdsess_t));
		sess->fd = mb->fd;
		sess->id = mb->id;
		sess->ip = *((uint32_t*)mb->data);
		sess->port = *((uint16_t *)mb->data[4]);
		save_fd(sess);
	}
	return 0;
}

int do_blk_close(mem_block_t *blk)
{
	fdsess_t *sess = get_fd(blk->fd);
	if (!sess) {
		ERROR(0, "[fd=%u] have closed", blk->fd);
		return -1;
	}
	//处理接口关闭回调
	so.on_cli_closed(blk->fd);
	
	remove_fd(blk->fd);

	return 0;
}

int do_proc_mcast(int fd)
{
	static char buf[MCAST_MSG_LEN];
	return 0;
}

int do_proc_svr(int fd) 
{
	return 0;
}

int do_proc_pipe(int fd) 
{
	static char pipe_buf[PIPE_MSG_LEN];
	while (read(fd, pipe_buf, PIPE_MSG_LEN) == PIPE_MSG_LEN) {}
	return 0;
}

void close_cli(int fd)
{
	fdsess_t *sess = get_fd(fd);
	if (!sess) {
		return ;
	}

	mem_block_t blk;
	blk.id = sess->id;
	blk.len = blk_head_len;
	blk.type = BLK_CLOSE;
	blk.fd = fd;
	shmq_push(workmgr.works[blk.id].sq, &blk, NULL);
	//do_blk_close(&blk); 不要重复执行
}
