/*
 * =====================================================================================
 *
 *       Filename:  test_log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月04日 22时26分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  XiaoNan, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include "log.h"

int main(int argc, char* argv[])
{
	int ret = log_init("log", LOG_LV_DEBUG, 1024 * 10, 1000, "test"); 
	if (ret == -1) {
		printf("error log init\n");
		return 0;
	}

	int i = 0;
	int coef = atoi(argv[1]);
	for (i = 0; i < 1000000; ++i) {
		DEBUG(0, "%d %d", coef, i * coef);
		if (i % 10 == 0) {
			sleep(1);
		}
	}
	log_fini();
	return 0;
}
