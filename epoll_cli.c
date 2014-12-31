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
	char data[];
} __attribute__((packed))proto_pkg_t;

void gen_str(char buf[], int n)
{
	int i;
	for (i = 0; i < n; i++) {
		buf[i] = (char)('a' + rand() % 26);
	}
}

int main(int argc, char* argv[]) 
{
	srand(time(NULL));
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		printf("%s\n", strerror(errno));
	}

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9000);
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
	struct epoll_event *evs = (struct epoll_event *)malloc(sizeof(struct epoll_event) * 10);	
	int done = 0;

	
	while (!done) {
		int i;

		int n = epoll_wait(epfd, evs, 1024, 100);
		if (n == -1 && errno != EINTR) {
			printf("%s", strerror(errno));
			return 0;
		}


		for (i = 0; i < n; ++i) {
			int fd = evs[i].data.fd;
			printf("fd=%u type=%u\n", fd, evs[i].events);
			if (evs[i].events && EPOLLIN) {
				char recvbuf[1024];
				int ret = recv(fd, recvbuf, 1024, 0);
				if (ret == 0) {
					printf("fd closed");
					return 0;
				} else {
					proto_pkg_t *msg = (proto_pkg_t *)recvbuf;
					printf("recv: %d,%d,%d,%d,%d,%s\n", 
							msg->id, 
							msg->seq,
							msg->cmd, 
							msg->ret, 
							msg->len,
							msg->data
							);
				}
			} else if (evs[i].events && EPOLLOUT) {

			}
		}
		getchar();
		char buf[1024];
		char input[200] = {'\0'};
		//int num = rand() % 100 + 1;
		int num = 30;
		gen_str(input, num);
		printf("send: %s:%lu\n", input, strlen(input));
//		scanf("%s", input);
		proto_pkg_t *pkg = (proto_pkg_t *)buf;	
		pkg->id = 1;
		pkg->seq  = 2;
		pkg->cmd = 3;
		pkg->ret = 4;

		pkg->len = sizeof(proto_pkg_t) + strlen(input) + 1;
		input[strlen(input)] = '\0';
		memcpy(pkg->data, input, strlen(input) + 1);
		for (i = 0; i < 10; ++i) {
			send(fd, buf, pkg->len, 0);
		}
		sleep(1);
	}

	free(evs);
	close(epfd);
	close(fd);

	return 0;
}
