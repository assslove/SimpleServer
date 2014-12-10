/*
 * =====================================================================================
 *
 *       Filename:  test_mem_queue_fork.c
 *
 *    Description:  测试一父一子通过管道通信数据
 *					父写子读
 *
 *        Version:  1.0
 *        Created:  12/10/2014 11:57:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>

#include "mem_queue.h"

int main(int argc, char *argv[])
{
	mem_queue_t q;
	mq_init(&q, 1024);
	int pid = fork();	
	if (pid == -1) {
		printf("error fork");
		mq_fini(&q, 1024);
		return 0;
	} else if (pid == 0) {
		close(q.pipefd[1]);
		char buf[2];
		for (;;) {
			while (read(q.pipefd[0], buf, 1) > 0) {
				mem_block_t *b;
				while ((b = mq_get(&q)) != NULL) {
					printf("\nchild --%s\n", b->data);
					mq_display(&q);
					mq_pop(&q);
					mq_display(&q);
					printf("\nchild --\n");
				}
			}
		}

		mq_fini(&q, 1024);
		return 0;
	} 

	close(q.pipefd[0]);
	int i = 0;
	char c = getchar();
	while (c != 'q') {
		for (i = 0; i < 10; i++) {
			mem_block_t b;
			char *data = (char*)malloc(30);
			memcpy(data, "hello, world", 30);
			b.len = 100;
			printf("\nparent --\n");
			mq_display(&q);
			mq_push(&q, &b, data);
			mq_display(&q);
			printf("\nparent --\n");
			sleep(1);
			free(data);
		}
		c = getchar();
	}

	mq_fini(&q, 1024);

	return 0;
}
