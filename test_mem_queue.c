/*
 * =====================================================================================
 *
 *       Filename:  test_mem_queue.c
 *
 *    Description:	test mem_queue
 *
 *        Version:  1.0
 *        Created:  2014年12月09日 23时25分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  XiaoNan, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "mem_queue.h"

int main(int argc, char* argv[]) 
{
	mem_queue_t q;
	mq_init(&q, 1025);
	mq_display(&q);

	int i = 0; 
	//push
	for (i = 0; i < 12; i++) {
		mem_block_t blk;
		char *str = (char *)malloc(30); 
		memcpy(str, "hello, world", 30);
		blk.data = str;
		blk.len = 100;
		mq_push(&q, &blk);
		mq_display(&q);
	}
	printf("--------------------\n");
	//pop
	mem_block_t *b = NULL;
	while ((b = mq_get(&q)) != NULL) {
		printf("%s\n", b->data);
		mq_pop(&q);
		mq_display(&q);
	}

	mq_display(&q);
	printf("--------------------\n");
	for (i = 0; i < 12; i++) {
		mem_block_t blk;
		char *str = (char *)malloc(30); 
		memcpy(str, "hello, world", 30);
		blk.data = str;

		blk.len = 100;
		mq_push(&q, &blk);
		mq_display(&q);
	}
	printf("--------------------\n");

	return 0;
}
