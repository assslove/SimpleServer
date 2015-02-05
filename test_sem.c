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
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string.h>

int sem_id;

int sem_lock() 
{
	struct sembuf sem_p;
	sem_p.sem_num = 0;
	sem_p.sem_op = -1;
	sem_p.sem_flg = SEM_UNDO;

	if (semop(sem_id, &sem_p, 1) == -1) {
		fprintf(stderr, "sem lock failed\n");		
		return -1;
	}

	return 0;
}

int sem_unlock() 
{
	struct sembuf sem_v;
	sem_v.sem_num = 0;
	sem_v.sem_op = 1;
	sem_v.sem_flg = SEM_UNDO;

	if (semop(sem_id, &sem_v, 1) == -1) {
		fprintf(stderr, "sem lock failed\n");		
		return -1;
	}

	return 0;

}

int main(int argc, char* argv[])
{
	int semkey = 10000;
	sem_id = semget(semkey, 1, IPC_CREAT | 0666);
	if (sem_id == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		return 1;
	}

	int pid = fork();
	if (pid == 0) { 

		return 0;
	} else if (pid > 0) { //父进程
		
	} else {
		fprintf(stderr, "cannot fork\n");
	}

	return 0;
}
