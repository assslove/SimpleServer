/*
 * =====================================================================================
 *
 *       Filename:  net_util.c
 *
 *    Description:  real net util
 *
 *        Version:  1.0
 *        Created:  2014年11月24日 22时26分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou , houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "net_util.h"
#include "util.h"
#include "fds.h"
#include "log.h"
#include "global.h"

int set_sock_snd_timeo(int sockfd, int millisec)
{
	struct timeval tv;

	tv.tv_sec  = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;

	return setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int set_sock_rcv_timeo(int sockfd, int millisec)
{
	struct timeval tv;

	tv.tv_sec  = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;

	return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int safe_tcp_recv_n(int sockfd, void* buf, int total)
{
	assert(total > 0);

	int recv_bytes, cur_len = 0;

	for (recv_bytes = 0; recv_bytes < total; recv_bytes += cur_len)	{
		cur_len = recv(sockfd, buf + recv_bytes, total - recv_bytes, 0);
		if (cur_len == 0) { 
			return 0;
		} else if (cur_len == -1) { // errno 
			if (errno == EINTR) {
				cur_len = 0;
			} else if (errno == EAGAIN || errno == EWOULDBLOCK)	{
				return recv_bytes;
			} else {
				return -1;
			}
		}
	}

	return recv_bytes;
}

int safe_tcp_send_n(int sockfd, const void* buf, int total)
{
	assert(total > 0);

	int send_bytes, cur_len;

	for (send_bytes = 0; send_bytes < total; send_bytes += cur_len) {
		cur_len = send(sockfd, buf + send_bytes, total - send_bytes, 0);
		if (cur_len == -1) {
			if (errno == EINTR) {
				cur_len = 0;
			} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return send_bytes;
			} else {
				return -1;
			}
		}
	}

	return send_bytes;
}

int safe_socket_listen(const char* ipaddr, in_port_t port, int type, int backlog, int bufsize)
{
	TRACE(0, "backlog=%d,bufsize=%u", backlog, bufsize);
	assert((backlog > 0) && (bufsize > 0) && (bufsize <= (10 * 1024 * 1024)));

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family  = AF_INET;
	servaddr.sin_port    = htons(port);
	if (ipaddr) {
		inet_pton(AF_INET, ipaddr, &servaddr.sin_addr);
	} else {	
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	int listenfd;
	if ((listenfd = socket(AF_INET, type, 0)) == -1) {
		return -1;
	}

	int err;
	if (type != SOCK_DGRAM) {
		int reuse_addr = 1;	
		err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
		if (err == -1) {
			goto ret;
		}
	}

	err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}
	err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}

	err = bind(listenfd, (void*)&servaddr, sizeof(servaddr));
	if (err == -1) {
		goto ret;
	}

	if ((type == SOCK_STREAM) && (listen(listenfd, backlog) == -1)) {
		err = -1;
		goto ret;
	}

ret:
	if (err) {
		err      = errno;
		close(listenfd);
		listenfd = -1;
		errno    = err;		
	}

	return listenfd;
}

int safe_tcp_accept(int sockfd, struct sockaddr_in* peer, int nonblock)
{
	int err;
	int newfd;

	for ( ; ; ) {
		socklen_t peer_size = sizeof(*peer);
		newfd = accept(sockfd, (void*)peer, &peer_size);
		if (newfd >= 0) {
			break;
		} else if (errno != EINTR) {
			return -1;
		}
	}

	if (nonblock && (set_io_nonblock(newfd, 1) == -1)) {
		err   = errno;
		close(newfd);
		errno = err;
		return -1;
	}

	return newfd;
}

int safe_tcp_connect(const char* ipaddr, in_port_t port, int timeout, int nonblock)
{
	struct sockaddr_in peer;

	memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = htons(port);
	if (inet_pton(AF_INET, ipaddr, &peer.sin_addr) <= 0) {
		return -1;
	}

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if ( sockfd == -1 ) {
		return -1;
	}

	if (timeout > 0) {
		set_sock_snd_timeo(sockfd, timeout * 1000);
	}
	if (connect(sockfd, (void*)&peer, sizeof(peer)) == -1) {
		close(sockfd);
		return -1;
	}
	if (timeout > 0) {
		set_sock_snd_timeo(sockfd, 0);
	}
	set_io_nonblock(sockfd, nonblock);

	return sockfd;
}

int add_fd_to_epinfo(int epfd, int fd, int events)
{
	set_io_nonblock(fd, 1);

	struct epoll_event event;
	event.data.fd = fd;
	event.events = events | EPOLLET;

	return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);	
}

int mod_fd_to_epinfo(int epfd, int fd, int events)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = events | EPOLLET;

	return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);	
}

int add_pfd_to_epinfo(int epfd, void *pfd, int events)
{
	fdsess_t *sess = pfd;
	set_io_nonblock(sess->fd, 1);

	struct epoll_event event;
	event.data.ptr = pfd;
	event.events = events | EPOLLET;

	return epoll_ctl(epfd, EPOLL_CTL_ADD, sess->fd, &event);	
}

int mod_pfd_to_epinfo(int epfd, void *pfd, int events)
{
	fdsess_t *sess = pfd;
	struct epoll_event event;
	event.data.ptr = pfd;
	event.events = events | EPOLLET;

	return epoll_ctl(epfd, EPOLL_CTL_MOD, sess->fd, &event);	
}


int send_to_cli(struct fdsess *sess, const void *msg, int const len)
{
	mem_block_t blk;

	blk.id = sess->id;
	blk.fd = sess->fd;
	blk.type = BLK_DATA;
	blk.len = len + blk_head_len;

	if (mq_push(&workmgr.works[blk.id].sq, &blk, msg) == -1) {
		ERROR(0, "%s error [len=%d]", __func__, len);
		return -1;
	}

	return 0;
}

int send_to_serv(int fd, void *msg, int len)
{
	return do_fd_send(fd, msg, len);
}

void free_buff(fd_buff_t *buff)
{
	if (buff->sbf) {
		free(buff->sbf);
		buff->sbf = NULL;
	}

	if (buff->rbf) {
		free(buff->rbf);
		buff->rbf = NULL;
	}

	buff->slen = 0;
	buff->rlen = 0;
}

int do_fd_send(int fd, void *data, int len)
{
	fd_buff_t *buff = &epinfo.fds[fd].buff;	
	int is_send = 0;
	if (buff->slen > 0) {
		if (do_fd_write(fd) == -1) { //fd断了
			do_fd_close(fd);
			return -1;	
		}
		is_send = 1;
	}
	
	int send_len = 0;
	if (buff->slen == 0) { //发送缓冲区没有数据发送
		send_len = safe_tcp_send_n(fd, data, len);
		if (send_len == -1) {
			ERROR(0, "write fd error[fd=%u, err=%s]", fd, strerror(errno));
			do_fd_close(fd);
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
			buff->sbf = (char *)realloc(buff->sbf, buff->slen + left_len);
			if (!buff->sbf) {
				ERROR(0, "realloc err, [err=%s]", strerror(errno));
				return -1;
			}
			buff->sbf_size = left_len + buff->slen;
		}

		memcpy(buff->sbf + buff->slen, (char *)data + send_len, left_len);
		buff->slen += left_len;

		if (setting.max_buf_len && setting.max_buf_len < buff->slen) { //如果大于最大发送缓冲区
			ERROR(0, "sendbuf is exceeded max[fd=%d,buflen=%d,slen=%d]", fd, setting.max_buf_len, buff->slen);
			do_fd_close(fd);
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
	fd_buff_t *buff = &epinfo.fds[fd].buff;
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


int do_fd_close(int fd)
{
	if (epinfo.fds[fd].type == fd_type_null) {
		return 0;
	}

	mem_block_t blk;
	blk.id = epinfo.fds[fd].idx;
	blk.fd = fd;
	blk.type = BLK_CLOSE;
	blk.len = blk_head_len;

	mq_push(&workmgr.works[blk.id].sq, &blk, NULL);

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

	return 0;
}

void do_add_to_readlist(int fd) 
{
	if (!epinfo.fds[fd].flag) {
		list_add_tail(&epinfo.fds[fd].node, &epinfo.readlist);
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

void do_add_to_closelist(int fd) 
{
	do_del_from_readlist(fd);
	if (!(epinfo.fds[fd].flag & CACHE_CLOSE)) {
		list_add_tail(&epinfo.fds[fd].node, &epinfo.readlist);
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


