/*
 * =====================================================================================
 *
 *       Filename:  share_mem.c
 *
 *    Description:  共享内存测试
 *
 *        Version:  1.0
 *        Created:  12/05/2014 11:54:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* @brief 内存队列
 */
typedef struct mem_queue {
	void* ptr;
	int length;
	int blk_cnt; 
} mem_queue_t;

/* @brief 内存块
 */
typedef struct mem_block {
	int fd;
	int start;
	int length;
} mem_block_t;

int main(int argc, char* argv[])
{ 
	mem_queue_t queue;
	void *ptr = mmap((void*)-1, 10240, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, 0, 0);	
	if (ptr == MAP_FAILED) {
		printf("map faild");
		return 0;
	}

	int queue_len = 12;
	int blk_len = 12;

	queue.ptr = ptr;
	queue.length = 10240;
	queue.blk_cnt = 0;
	memcpy(ptr, &queue, queue_len);

	int pid = fork();
	if (pid < 0) { //error
		printf("error fork");
		return 0;
	} else if (pid == 0) { //child
		mem_queue_t *tmp = ptr;
		sleep(1);
		mem_block_t blk;	
		memcpy(&blk, tmp->ptr + 12, 12);
		char buf[32] = {'\0'};	
		memcpy(buf, tmp->ptr + 12 + 12, blk.length);
		printf("child:%s:%d:%d\n", buf, tmp->length, tmp->blk_cnt);

		munmap(ptr, 10240);
		exit(0);
	}

	mem_block_t blk ;
	blk.fd = 1;
	blk.start = 12;
	blk.length = 12;

	memcpy(queue.ptr + queue_len, &blk, 12); 
	memcpy(queue.ptr + queue_len + blk_len, "hello,world", 12); 

	mem_queue_t *tmp = (mem_queue_t *)ptr;
	tmp->length -= blk_len + queue_len + blk.length;
	tmp->blk_cnt = 1;

	sleep(2);
	munmap(queue.ptr, 10240);

	return 0;
}
