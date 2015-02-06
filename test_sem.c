/*
 * =====================================================================================
 *
 *       Filename:  test_sem.c
 *
 *    Description:  测试信号量，用来进行实现进程间同步
 *
 *        Version:  1.0
 *        Created:  2015年02月03日 14时53分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>           /*  For O_* constants */
#include <sys/stat.h>        /*  For mode constants */
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	sem_t *sem = sem_open("bin123", O_CREAT, 0666, 1);
	if (sem == SEM_FAILED) {
		fprintf(stderr, "sem open error\n");
		return 1;
	}

	int value;

	sem_getvalue(sem, &value);
	if (value == 0) {
		printf("sem value is zero\n");
		fflush(NULL);
		sem_post(sem);
	}

	int *ptr = mmap((void *)-1, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "mmap failed\n");
		return 1;
	}

	*ptr = 0;

	int pid = fork();
	if (pid == 0) { 
		while (1) {
			sem_wait(sem);
			++(*ptr);
			printf("child %u\n", *ptr);
			fflush(NULL);
			sem_post(sem);
		}
		sem_close(sem);
		munmap(ptr, 1024);
		return 0;
	} else if (pid > 0) { //父进程
		while (1) {
			sem_wait(sem);
			++(*ptr);
			printf("parent %u\n", *ptr);
			fflush(NULL);
			sem_post(sem);
		}
	} else {
		fprintf(stderr, "cannot fork\n");
	}

	sem_close(sem);
	sem_unlink("bin123");
	munmap(ptr, 1024);
	return 0;
}
