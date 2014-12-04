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
	DEBUG(100021, "%d: test", 1111);
	return 0;
}
