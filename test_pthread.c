/*
 * =====================================================================================
 *
 *       Filename:  test_pthread.c
 *
 *    Description:  对线程的测试
 *
 *        Version:  1.0
 *        Created:  2014年12月31日 09时48分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>


void* produce(void *arg)
{
	static int i = 0;
	while (i != 10) {
		sleep(1);
		++i;
		printf("child %u\n", i);
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	pthread_t tid;

	int ret = pthread_create(&tid, NULL, produce, NULL);
	if (ret != 0) {
		errno = ret;
		printf("create pthread error! %s", strerror(errno));
	}

	printf("parent tid=%u\n", (uint32_t)tid);

	pthread_join(pid, NULL);

	sleep(20);

	return 0;
}
