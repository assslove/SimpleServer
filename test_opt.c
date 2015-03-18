/*
 * =====================================================================================
 *
 *       Filename:  test_opt.c
 *
 *    Description:  用于测试linux设置参数列表
 *
 *        Version:  1.0
 *        Created:  2015年03月18日 19时16分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	struct option opts[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"set", required_argument, NULL, 's'}
	};

	int opt = 0;
	while ((opt = getopt_long(argc, argv, "s:hv", opts, NULL)) != -1) {
		switch (opt) {
			case 'h':
				printf("help\n");
				break;
			case 'v':
					printf("version\n");
				break;
			case 's':
					printf("set %s", optarg);
				break;
			default:
				printf("error usage\n");
				break;
		}
	}

	return 0;
}
