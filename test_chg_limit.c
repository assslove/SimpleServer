/*
 * =====================================================================================
 *
 *       Filename:  test_chg_limit.c
 *
 *    Description:  test chg limit
 *
 *        Version:  1.0
 *        Created:  2014年12月10日 22时35分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  XiaoNan, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <sys/resource.h>


int init_rlimit()
{
	struct rlimit rlim;
	rlim.rlim_cur = rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);
}

int main(int argc, char* argv[])
{
	init_rlimit();
	return 0;
}
