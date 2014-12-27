/*
 * =====================================================================================
 *
 *       Filename:  test_signal.c
 *
 *    Description:  对信号的测试
 *
 *        Version:  1.0
 *        Created:  12/27/2014 09:47:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void test_signal(int no) 
{
	int pid;
	int status;
	pid = waitpid(-1, &status, 0);
	if (pid > 0) {
		printf("%d have stop\n", pid);
		create_work();
	} else {

	}
}

int create_work() 
{
	int pid = fork();
	if (pid == 0) {
		int n = 0;
		while(1) {
			sleep(1);
			printf("child %d\n", ++n);
		};
	} 

	return pid;
}

int main(int argc, char *argv[])
{
	sigset_t sigs, oset;
	sigemptyset(&sigs);
	struct sigaction act, oact;	
	act.sa_handler = test_signal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGCHLD, &act, &oact) < 0) {
		printf("error add signal\n");
		return 0;
	}

	int pid = 0;
	pid = create_work();
	if (pid < 0) {
		return 0;
	}
	
	int n = 0;
	while(1) {
		sleep(2);
		printf("parent %d\n", ++n);
	}

	return 0;
}
