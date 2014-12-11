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
#include <sys/mman.h>
#include <arpa/inet.h>

#include "net_util.h"
#include "log.h"
#include "mem_queue.h"

enum RTYPE {
	SUCCESS = 0,
	ERROR = -1 
};

work_t works[4];
int chl_pids[1024] = {0};

typedef struct svr_setting {
	int nr_max_event;	// 最大的事件类型 epoll_create ms不需要这个参数了
	int mem_queue_len;	// 共享内存队列长度
	int max_msg_len;	// 最大消息长度
	int max_buf_len;	//发送(接收)缓冲区最大长度 超过报错
} svr_setting_t;


epoll_info_t epinfo;
svr_setting_t setting = {
	1024, 
	1024 * 1024 * 10
};

int main(int argc, char* argv[]) 
{	
	//chg limit
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

	int i = 0;
	for (i = 0; i < 1; i++) {
		int pid = fork();
		if (pid == -1) {
			ERROR(0, "%s", strerror(errno));
			//关闭系统资源
			return 0;
		} else if (pid == 0) { //子进程
			//释放主进程资源
			close(epinfo.epfd); 
			close(listenfd);
			close(epinfo.epfd);

			epinfo.epfd = epoll_create(1024);
			close(works[0].send_q.pipefd[0]);
			close(works[0].recv_q.pipefd[1]);

			add_fd_to_epinfo(epinfo.epfd, works[0].recv_q.pipefd[0], EPOLLIN);
			
			int stop = 0;
			while (!stop) {
				int nr = epoll_wait(epinfo.epfd, epinfo.evs, 1024, 100);
				int i;
				for (i = 0; i < nr; i++) {
					int fd = epinfo.evs[i].data.fd;
					DEBUG(fd, "msg from %d", fd);
				}
			}

			return 0;
		} 

		chl_pids[i]	= pid; //赋值pid

		//int queue_len = 1024 * 1024;
		//works[i].send_q.info = (mem_head_t *)mmap((void *)-1, 1024 * 1024, PROT_READ | PROT_WRITE, \
				//MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		//works[i].send_q.len = queue_len;
		//works[i].send_q.info->head = sizeof(mem_head_t);
		//works[i].send_q.info->tail = sizeof(mem_head_t);
		//works[i].send_q.info->blk_cnt = 0;
		//pipe(works[i].send_q.pipefd);
		//close(works[i].send_q.pipefd[1]);

		//works[i].recv_q.info = (mem_head_t *)mmap((void *)-1, 1024 * 1024, PROT_READ | PROT_WRITE, \
				//MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		//works[i].recv_q.len = queue_len;
		//works[i].recv_q.info->head = sizeof(mem_head_t);
		//works[i].recv_q.info->tail = sizeof(mem_head_t);
		//works[i].recv_q.info->blk_cnt = 0;

		//pipe(works[i].recv_q.pipefd);
		//close(works[i].recv_q.pipefd[0]);

		//add_fd_to_epinfo(epinfo.epfd, works[i].send_q.pipefd[1]);
	}

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
					char ip[32];
					inet_ntop(AF_INET, &(cliaddr.sin_addr), ip, 32);
					DEBUG(fd, "cli %s connect", ip);
					add_fd_to_epinfo(epinfo.epfd, connfd, EPOLLIN);
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

					send(fd, buf, 32, 0);					
					char noti[] = {'r'};

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

