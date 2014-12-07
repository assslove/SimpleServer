/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  epoll 	svr 
 *					muti process
 *					provide some interface for man
 *
 *        Version:  1.0
 *        Created:  04/06/2014 11:38:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> /* basic socket definition */
#include <netinet/in.h> /* sockaddr_in and other internet def */
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>   
#include <string.h>   
#include <sys/epoll.h>

#include "net_util.h"
#include "log.h"

enum RTYPE {
	SUCCESS = 0,
	ERROR = -1 
};

/* @brief 工作进程配置项
 */
typedef struct {
	uint32_t work_id;
	char ip[32];
	uint16_t port;
	uint8_t proto_type; 
}__attribute__((packed)) work_conf_t;

typedef struct {
	uint32_t id;
	int fd;
	uint8_t type;
	void (*callback)(int fd, void* arg);
	void *arg;
} __attribute__((packed)) fd_wrap_t;

typedef struct {
	int epfd;
	struct epoll_event *evs;
	fd_wrap_t *fds;
	int max_fd;
	int max_ev;
}__attribute__((packed)) epoll_info_t;

work_conf_t work_confs[] = {
	{1, "127.0.0.1", 10001, 0},
	{2, "127.0.0.1", 10002, 0},
	{3, "127.0.0.1", 10003, 0},
	{4, "127.0.0.1", 10004, 0}
};

int chl_pids[1024] = {0};

typedef struct svr_setting {
	int nr_max_event; //
} svr_setting_t;


epoll_info_t epinfo;
svr_setting_t setting = {
	1024 
};

int main(int argc, char* argv[]) 
{	
	epinfo.epfd = epoll_create(setting.nr_max_event);
	if (epinfo.epfd == -1) {
		ERROR(0, "%s", strerror(errno));
		return ERROR;
	}

	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		ERROR(0, "listen socket error\n");
		return ERROR;
	}

	struct sockaddr_in servaddr;	
	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(8000);

	//设置socket
	int flag = 1;
	int err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	if (err == -1) {
		ERROR(0, "%s", strerror(errno));
		return ERROR;
	}
	
	int bufsize = 1024 * 1024; //1M
	err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));
	if (err == -1) {
		ERROR(0, "%s", strerror(errno));
		return ERROR;
	}
	err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	if (err == -1) {
		ERROR(0, "%s", strerror(errno));
		return ERROR;
	}

	err = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (err == -1) {
		ERROR(0, "%s", strerror(errno));
		return ERROR;
	}

	epinfo.evs = (struct epoll_event *)calloc(setting.nr_max_event, sizeof(struct epoll_event));		

	struct epoll_event event;	
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epinfo.epfd, EPOLL_CTL_ADD, listenfd, &event);

	set_io_nonblock(listenfd, 1);
	err = listen(listenfd, 1024);
	if (err == -1) {
		ERROR(0, "%s", strerror(errno));
		return ERROR;
	}

	DEBUG(0, "listen on 8000\n");

	int stop = 0;
	while (!stop) {
		int nr = epoll_wait(epinfo.epfd, epinfo.evs, setting.nr_max_event, 100);
		int i = 0;
		for (i = 0; i < nr; i++) {
			int fd = epinfo.evs[i].data.fd;
			if (epinfo.evs[i].events && EPOLLIN) { //监听端口
				if (fd == listenfd) {
					struct sockaddr_in cliaddr;
					int clilen = sizeof(cliaddr);
					int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);	
					if (connfd == -1) {
						ERROR(0, "%s", strerror(errno));
						continue;
					}

					DEBUG(fd, "cli %s connect", inet_ntoa(cliaddr.sin_addr));

					set_io_nonblock(connfd, 1);

					struct epoll_event event;
					event.data.fd = connfd;
					event.events = EPOLLIN | EPOLLET;

					epoll_ctl(epinfo.epfd, EPOLL_CTL_ADD, connfd, &event);
				} else { //读数据
					int n, buf[1024] = {'\0'};
read_again:
					if ((n = recv(fd, buf, 1024, 0)) > 0) { //阻塞的
						buf[n] = '\0';
						DEBUG(0, "read info %s", buf);
					} else if (n < 0) {
						if (errno == EINTR) {
							goto read_again;
						} else {
							break;
						}
					}

					//epinfo.evs[i].events = EPOLLOUT | EPOLLET;
					//epoll_ctl(epinfo.epfd, EPOLL_CTL_MOD, fd, &(epinfo.evs[i]));

					send(fd, buf, 32, 0);					

					DEBUG(fd, "send to cli[%s]", buf);
				}
			} else if (epinfo.evs[i].events && EPOLLOUT) { //写数据
				epinfo.evs[i].events = EPOLLIN | EPOLLET;
				epoll_ctl(epinfo.epfd, EPOLL_CTL_MOD, fd, &(epinfo.evs[i]));
				DEBUG(fd, "hell");
			}
		}
	}

	free(epinfo.evs);
	close(epinfo.epfd);
	close(listenfd);

	return 0;
}

