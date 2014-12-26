/*
 * =====================================================================================
 *
 *       Filename:  epoll_cli.c
 *
 *    Description:  使用epoll cli进行测试
 *
 *        Version:  1.0
 *        Created:  12/19/2014 05:44:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>

typedef struct proto_pkg {
	int len;
	int id;
	int seq;
	int cmd;
	int ret;
	uint8_t data[];
} __attribute__((packed))proto_pkg_t;

int main(int argc, char* argv[]) 
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		printf("%s\n", strerror(errno));
	}

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

	int ret = connect(fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));

	if (ret == -1) {
		printf("%s", strerror(errno));
		return 0;
	}

	int epfd = epoll_create(1024);
	if (epfd == -1) {
		printf("create failed");
		return 0;
	}

	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
	struct epoll_event *evs = (struct epoll_event *)malloc(sizeof(struct epoll_event) * 1024);	
	int done = 0;
	while (!done) {
		int i;
		int n = epoll_wait(epfd, evs, 1024, 10);
		if (n == -1 && errno != EINTR) {
			printf("%s", strerror(errno));
			return 0;
		}

		for (i = 0; i < n; ++i) {
			int fd = evs[i].data.fd;
			printf("fd=%u type=%u\n", fd, evs[i].events);
			switch (evs[i].events) {
				case EPOLLIN:
					break;
				case EPOLLOUT:
					break;
			}
		}
	}

	free(evs);
	close(epfd);
	close(fd);

	return 0;
}
