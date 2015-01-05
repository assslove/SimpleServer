/*
 * =====================================================================================
 *
 *       Filename:  ev_serv.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年01月05日 14时15分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "log.h"

/* @brief 定义包
*/
typedef struct proto_pkg {
	int len;
	int id;
	int seq;
	int cmd;
	int ret;
	uint8_t data[];
} __attribute__((packed)) proto_pkg_t;

void readcb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *input = bufferevent_get_input(bev);
	struct evbuffer *output= bufferevent_get_input(bev);

	evbuffer_add_buffer(output, input);

	evutil_socket_t fd = *(evutil_socket_t *)user_data;

	uint32_t total = evbuffer_get_length(input);
	INFO(0, "recv read event [total=%u, fd=%d]", total, fd);	

	if (total < 4) {
		return ;
	}
	
	uint32_t len = 0;
	char buff[102400];
	uint32_t used = 0;

	while (used < total) {
		evbuffer_copyout(input, &len, 4);
		evbuffer_remove(input, buff, len);

		proto_pkg_t *pkg = (proto_pkg_t *)buff;
		DEBUG(pkg->id, "online len=%u,id=%u,seq=%u,cmd=%u,ret=%u, msg=%s", pkg->len, pkg->id, pkg->seq, pkg->cmd, pkg->ret, (char *)pkg->data);

//		evbuffer_drain(input, len);
		used += len;	
	}
	evbuffer_write(output, fd);
}

void writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output= bufferevent_get_input(bev);

	uint32_t total = evbuffer_get_length(output);
	evutil_socket_t fd = *(evutil_socket_t *)user_data;
	INFO(0, "recv write event [total=%u, fd=%d]", total, fd);	

	evbuffer_write(output, fd);
}

void event_cb(struct bufferevent *bev, short events, void *user_data)
{
	if (events | BEV_EVENT_EOF) {
		ERROR(0, "connection closed");
	} else if (events | BEV_EVENT_ERROR) {
		ERROR(0, "connect err [%s]", strerror(errno));
	}

	bufferevent_free(bev);
}

void signal_cb(evutil_socket_t fd, short event, void* user_data)
{
	struct event_base *base = user_data;	
	struct timeval delay = {2, 0};

	INFO(0, "catch sigint, ev_serv will stop after 2 seconds");

	event_base_loopexit(base, &delay);
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		ERROR(0, "error new bufferevent");
		event_base_loopbreak(base);
		return ;
	}

	bufferevent_setcb(bev, readcb, writecb, event_cb, &fd);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	INFO(0, "cli have accept [fd=%u]", fd);
}

int main(int argc, char* argv[])
{
	struct event_base *base;
	struct evconnlistener *listener;

	base = event_base_new();
	if (!base) {
		ERROR(0, "could not init libevent");
		return 1;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(0);
	servaddr.sin_port = htons(9000);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base, \
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 1024, \
			(struct sockaddr *)&servaddr, sizeof(servaddr));

	if (!listener) {
		ERROR(0, "could not init listener");
		return 1;
	}


	struct event *signal_ev = evsignal_new(base, SIGINT, signal_cb, (void*)base);
	if (!signal_ev || event_add(signal_ev, NULL)) {
		ERROR(0, "could not init/add signal event");
		return 1;
	}


	event_base_dispatch(base);
	evconnlistener_free(listener);
	event_free(signal_ev);
	event_base_free(base);

	INFO(0, "ev_serv have stop");
	return 0;
}
