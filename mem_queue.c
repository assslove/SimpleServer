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
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#include "mem_queue.h"
#include "util.h"

int mq_init(mem_queue_t *q, int size)
{
	q->len = size;	
	pipe(q->pipefd);

	set_io_nonblock(q->pipefd[0], 1);
	set_io_nonblock(q->pipefd[1], 1);

	q->ptr = (mem_head_t *)mmap((void *)-1, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);	
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
	close(q->pipefd[0]);
	close(q->pipefd[1]);
	return 0;
}

mem_block_t *mq_get(mem_queue_t *q)
{
	mem_head_t *ptr = q->ptr;		
get_again:
	if (ptr->head > ptr->tail) { 
		if (ptr->head > ptr->tail + q->blk_head_len) { 
			return blk_tail(q);
		}
	} else if (ptr->head < ptr->tail) {
		if (q->len < ptr->tail + q->blk_head_len) { //如果容纳不下一个块
			ptr->tail = q->mem_head_len;
			goto get_again;
		} else { //如果可以容纳一个块
			mem_block_t *blk = blk_tail(q);
			if (blk->type == BLK_ALIGN) { //如果是填充块 则调整位置
				ptr->tail = q->mem_head_len;
				goto get_again;
			} else {
				return blk;
			}
		}
	} else if (ptr->blk_cnt >= 10) { //相等情况下如果大于10块说明还有数据
		return blk_tail(q);	
	}

	return NULL;
}

int mq_push(mem_queue_t *q, mem_block_t *b, void *data)
{
	mem_head_t *ptr = q->ptr;
push_again:
	if (ptr->head >= ptr->tail) { //
		if (ptr->blk_cnt >= 10) {
			return -1;
		} 
		if (ptr->head + b->len > q->len) { //如果大于最大长度
			if (ptr->head + q->blk_head_len <= q->len) { //如果容下一个块头
				//填充
				mem_block_t *blk = blk_head(q);
				blk->type = BLK_ALIGN;
				blk->len = q->len; 
			}

			ptr->head = q->mem_head_len;		//调整到头部
			goto push_again;
		} else {
			mem_block_t *blk = blk_head(q);
			memcpy(blk, b, q->blk_head_len);
			memcpy(blk->data, data, b->len - q->blk_head_len);
			ptr->head += b->len;
			++ptr->blk_cnt;
			write(q->pipefd[1], q, 1);
		}
	} else if (ptr->head < ptr->tail) { 
		if (ptr->head + b->len > ptr->tail) {//full
			return -1;	
		} else {
			mem_block_t *blk = blk_head(q);
			memcpy(blk, b, q->blk_head_len);
			memcpy(blk->data, data, b->len - q->blk_head_len);
			ptr->head += b->len;
			++ptr->blk_cnt;
			write(q->pipefd[1], q, 1);
		}
	} 

	return 0;
}

void mq_pop(mem_queue_t *q)
{
	mem_head_t *ptr = q->ptr;
	mem_block_t *b = blk_tail(q);
	ptr->tail += b->len;
	--ptr->blk_cnt;
}

mem_block_t* blk_head(mem_queue_t *q)
{
	return (mem_block_t *)((char *)q->ptr + q->ptr->head);
}

mem_block_t* blk_tail(mem_queue_t *q)
{
	return (mem_block_t *)((char *)q->ptr + q->ptr->tail);
}

void mq_display(mem_queue_t *q)
{
	printf("blk_len=%d, head_len=%u, blk_cnt=%d, head=%d, tail=%d, len=%u\n", \
			q->blk_head_len, q->mem_head_len, q->ptr->blk_cnt, q->ptr->head, q->ptr->tail, q->len);
}
