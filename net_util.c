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

int set_io_isblock(int fd, int isblock)
{
	int val;
	if (isblock) {
		val = (O_NONBLOCK | fcntl(fd, F_GETFL));
	} else {
		val = (~O_NONBLOCK & fcntl(fd, F_GETFL));
	}
	return fcntl(fd, F_SETFL, val);
}

int set_sock_snd_timmo(int sockfd, int millisec)
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

	if (nonblock && (set_io_blockability(newfd, 1) == -1)) {
		err   = errno;
		close(newfd);
		errno = err;
		return -1;
	}

	return newfd;
}

int safe_tcp_connect(const char* ipaddr, in_port_t port, int timeout, int nonblock)
{
//	int err;
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

	// Works under Linux, although **UNDOCUMENTED**!!
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
	set_io_blockability(sockfd, nonblock);

	return sockfd;
}

