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
#include "mem_queue.h"
#include "log.h"
#include "conf.h"
#include "global.h"
#include "util.h"
#include "master.h"
#include "work.h"

int main(int argc, char* argv[]) 
{	
	//load conf
	load_conf();	
	//chg limit
	init_rlimit();
	//save args
	save_args(argc, argv);
	//chg serv name
	chg_proc_title("SimpleServer");
	//daemon mode
	daemon(0, 0);

	//handle signal
	//handle pipe
	
	//master_init
	master_init();

	int i = 0;
	for (; i < workmgr.nr_used; i++) {
		int pid = fork();
		if (pid <= 0) {
			ERROR(0, "create work fail[%d][%s]", i, strerror(errno));
			goto fail;	
		} else if (pid == 0) { //child
			work_init(i);
			work_dispatch(i);
			work_fini(i);
			exit(0);
		} else { //parent
			child_pid[i] = pid;
			parent_listen(i);
		}
	}

	//master loop
	master_dispatch();
	//master fini();
	master_fini();

	int i = 0;
	for (i = 0; i < 1; i++) {
		int pid = fork();
		if (pid == -1) {
			ERROR(0, "%s", strerror(errno));
			//关闭系统资源
			goto end;
		} else if (pid == 0) { //子进程
			chg_proc_title("SimpleServer-%d", 1);
			//释放主进程资源
			close(epinfo.epfd); 
			close(listenfd);
			close(epinfo.epfd);

			epinfo.epfd = epoll_create(1024);
			//close(works[0].send_q.pipefd[0]);
			//close(works[0].recv_q.pipefd[1]);

			//add_fd_to_epinfo(epinfo.epfd, works[0].recv_q.pipefd[0], EPOLLIN);
			
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

end:
	free(epinfo.evs);
	close(epinfo.epfd);
	close(listenfd);
fail:


	return 0;
}

