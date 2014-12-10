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
		mq_fini(&q, 1024);
	}
	return 0;
}
