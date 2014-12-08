/*
 * =====================================================================================
 *
 *       Filename:  mem_queue.c
 *
 *    Description:  共享队列设计
 *
 *        Version:  1.0
 *        Created:  12/07/2014 09:14:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <sys/mman.h>
#include <unistd.h>

#include "mem_queue_t.h"

int mq_init(mem_queue_t *q, int size)
{
	q->len = size;	
	pipe(q->pipefd);

	q->ptr = (mem_head_t *)mmap(-1, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);	
	if (q->ptr == MAP_FAILED) {
		return -1;
	}

	q->blk_head_len = sizeof(mem_block_t);
	q->mem_head_len = sizeof(mem_head_t);
	q->ptr->head = q->mem_head_len;
	q->ptr->tail = q->mem_head_len;

	return 0;
}

int mq_fini(mem_queue_t *q, int size)
{
	munmap(q->ptr, size);	
	//关闭管道
	close(pipefd[0]);
	close(pipefd[1]);
}

mem_block_t *mq_get(mem_queue_t *q)
{
	if (q->blk_cnt > 0) {
		mem_block_t *b = (mem_block_t *)((uint8_t *)(q->ptr) + q->ptr->tail); //copy head
		memcpy(b->data, (uint8_t *)(q->ptr) + p->ptr->tail + blk_head_len, b->len - blk_head_len); //copy data
		return b;
	}

	return NULL;
}

int mq_push(mem_queue_t *q, mem_block_t *b)
{
	//先对齐尾部
	mq_align_tail(q);		
}


mem_block_t* mq_pop(mem_queue_t *q)
{

}

int mq_align_head(mem_queue_t *q)
{

}

int mq_align_tail(mem_queue_t *q)
{
	
}
