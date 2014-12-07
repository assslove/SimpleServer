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

int mq_init(mem_queue_t **q, int size)
{
	
}

int mq_fini(mem_queue_t **q, int size)
{
	
}

int mq_push(mem_queue_t *q, mem_block_t *b)
{
	
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
