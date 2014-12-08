/*
 * =====================================================================================
 *
 *       Filename:  test_hashmap.c
 *
 *    Description:  测试hashmap 
 *
 *        Version:  1.0
 *        Created:  2014年12月08日 06时56分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  XiaoNan, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>

#include <glib.h>

void do_free_item(void* item)
{
	g_slice_free1(sizeof(int), item);
}

int main(int argc, char* argv[])
{
	GHashTable *table;
	table = g_hash_table_new_full(g_int_hash, g_int_equal, do_free_item, do_free_item);	

	int i = 0;
	for (i = 0; i = 100; i++) {
		int* key = g_slice_alloc(sizeof(int));
		int* value = g_slice_alloc(sizeof(int));
		
		*key = 0;
		*value = i + 1;

		g_hash_table_insert(table, key, value);
	}
	
	int key = 1;
	int* pval = g_hash_table_lookup(table, &key);
	if (pval == NULL) {
		printf("can find\n");
	} else {
		printf("%d\n", *pval);
	}

	g_hash_table_destroy(table);
	return 0;
}
