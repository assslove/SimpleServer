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


/* @brief 内存队列
 */
typedef struct mem_queue {
	void* ptr;
	int length;
	int start;
	int end;
} mem_queue_t;

/* @brief 内存块
 */
typedef struct mem_block {
	int data;
	int start;
	int length;
} mem_block_t;

int main(int argc, char* argv[])
{ 
	mem_queue_t queue = {
		NULL, 
		10240, 
		0, 
		0
	}; 
	queue.ptr = mmap((void*)-1, 10240, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, 0, 0);	

	munmap(queue.ptr, 10240);
	return 0;
}
