/*
 * =====================================================================================
 *
 *       Filename:  min_heap.c
 *
 *    Description:  最小堆实现 测试代码
 *
 *        Version:  1.0
 *        Created:  12/04/2014 12:21:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include "min_heap.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char* argv[]) 
{
	min_heap_t base;
	min_heap_ctor(&base);

	int i = 0;
	srand(time(NULL));
	for (i = 0; i < 100; i++) {
		my_struct_t *item = (my_struct_t *)malloc(sizeof(my_struct_t));
		item->data = rand() % 10000;
		min_heap_push(&base, item);
	}

	while (!min_heap_empty(&base)) {
		my_struct_t *item = min_heap_pop(&base);	
		printf("%d\n", item->data);
		free(item);
	}

	min_heap_dtor(&base);

	
	return 0;
}
