/*
 * =====================================================================================
 *
 *       Filename:  test_pipe.c
 *
 *    Description:  测试管道
 *
 *        Version:  1.0
 *        Created:  12/07/2014 09:47:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int pipefd[2];
	pipe(pipefd);
	
	int pid = fork(); 
	if (pid < 0) {
		return 0;
	} else if (pid == 0) {
		sleep(1);
		close(pipefd[1]);
		for (;;) {
			char buf[2] = {'\0'};
			read(pipefd[0], buf, 1);

			printf("%s\n", buf);
		}
		return 0;
	}

	close(pipefd[0]);

	for (;;) {
		char buf[] = {'c'};
		write(pipefd[1], buf, 1);
		sleep(1);
	}
	return 0;
}
