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
#include <strings.h>   /* bzero */

#include "epoll_util.h"
#include "net_util.h"

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
	int epoll_fd;
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

int child_pids[1024] = [0];
epoll_info_t epinfo;

int main(int argc, char* argv[]) 
{	
	ep_info = epoll_create(1024);
		
	int i = 0;
	uint32_t nwork = sizeof(work_confs) / sizeof(work_confs[0]);
	for (i = 0; i < nwork; i++) {
		work_conf_t *item = work_confs[i];

		int pid = fork();
		if (pid > 0) { //parent
			//开启epoll
		} else if (pid == 0) { //work
			//关闭资源
			
			//开启epoll
		} else { //释放资源
			goto end;		
		}
		
	}

	//父进程的状态
	
end:

	int listenfd, connfd;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		printf("socket error\n");
		return ERROR;
	}

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8000);

	int reuse_addr = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

	int ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0) {
		printf("socket bind error\n");
		return ERROR;
	}


	ret = listen(listenfd, 5);
	if (ret < 0) {
		printf("socket listen error\n");
		return ERROR;
	}

	int stop = 0;
	char buf[1024] = {'\0'};
	while (!stop) {
		int clilen = sizeof(struct sockaddr_in);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

		printf("connfd=%u\n", connfd);
		int n = 0;	

read_again:
		while ((n = recv(connfd, buf, 1024, 0)) > 0) { //阻塞的
			buf[n] = '\0';
			ret = send(connfd, buf, n, 0);	
			printf("read info %s\n", buf);
			if (ret < 0) {
				printf("write error");
				return ERROR;
			}
		}

		if (n < 0) {
			if (errno == EINTR) {
				goto read_again;
			} else {
				printf("socket read error\n");
				close(connfd);
				return ERROR;
			}
		}

		close(connfd);
	}

	close(listenfd);

	return 0;
}

