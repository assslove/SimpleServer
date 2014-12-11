/*
 * =====================================================================================
 *
 *       Filename:  test_load_conf.c
 *
 *    Description:  测试读取配置文件
 *
 *        Version:  1.0
 *        Created:  12/11/2014 03:59:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include <sys/mman.h>
#include <string.h>

void do_free_item(void* item)
{
	g_free(item);
	item = NULL;
}


GHashTable *conf;
int load_conf()
{
	conf = g_hash_table_new_full(g_str_hash, g_str_equal, do_free_item, do_free_item);
	int fd = open("./work.conf", 'r');
	char line[1024];
	char key[64];
	char value[128];
	char *ptr = (char *)mmap((void *)-1, 1024 * 10, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr == NULL) {
		printf("error mmap\n");
		return 0;
	}

	int start = 0;
	int end = 0;
	int i = 0;
	int is_note = 0;
	for (; ptr[i] != '\0'; i++) {
		if (ptr[i] == '#') {
			is_note = 1;
		} else if (ptr[i] == '\n') {
			end = i - 1;
			if (is_note) {
				start = i + 1;
				i = start - 1;
				is_note = 0;
				continue;
			} else {
				int len = end - start + 1;
				memcpy(line, ptr + start, len);
				if (len == 0) {
					start = i + 1;
					is_note = 0;
					continue;
				}
				line[len] = '\0';
				sscanf(line, "%s %s\n", key, value);
				g_hash_table_insert(conf, strdup(key), strdup(value));
				memset(key, 0, 64);
				memset(value, 0, 128);
			}
			start = i + 1;
			is_note = 0;
		}
	}

	munmap(ptr, 1024 * 10);
	close(fd);
}

void print(void *key, void *value, void *userdata)
{
	printf("%s-%s\n", (char *)key, (char *)value);
}
void print_conf()
{
	g_hash_table_foreach(conf, print, NULL);	
}

int main(int argc, char* argv[])
{
	load_conf();
	print_conf();
	g_hash_table_destroy(conf);
	return 0;
}
