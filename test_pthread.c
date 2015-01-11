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

int count = 0;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

void* produce(void *arg)
{
	int i;
	for (i = 0; i < 10000; ++i) { 
		pthread_mutex_lock(&count_mutex);
		printf("child tid=%u count=%u\n", (uint32_t)pthread_self(), count + 1);
		count = count + 1;
		pthread_mutex_unlock(&count_mutex);
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	pthread_t tid1, tid2;

	int ret = pthread_create(&tid1, NULL, produce, NULL);
	ret = pthread_create(&tid2, NULL, produce, NULL);
	if (ret != 0) {
		errno = ret;
		printf("create pthread error! %s", strerror(errno));
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	sleep(20);

	return 0;
}
