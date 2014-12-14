/*
 * =====================================================================================
 *
 *       Filename:  test_linklist.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2014 08:46:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>

#include "list.h"

typedef struct {
	int data;
	list_head_t list;
} node_t;

int main(int argc, char* argv[])
{
	list_head_t readlist;
	INIT_LIST_HEAD(&readlist);
	int i = 0;
	for (i = 0; i < 10; i++) {
		node_t *node = (node_t *)malloc(sizeof(node_t));
		node->data = i;
		list_add_tail(&node->list, &readlist);
	}
	
	printf("%d\n", list_empty(&readlist));;

	node_t *tmp;
	node_t *tmp2;
	list_for_each_entry_safe(tmp, tmp2, &readlist, list) {
		printf("%d\n", tmp->data);
		list_del(&tmp->list);
		free(tmp);
	}

	printf("%d\n", list_empty(&readlist));;
//	list_del(&readlist);

	return 0;
}
