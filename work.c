/*
 * =====================================================================================
 *
 *       Filename:  work.c
 *
 *    Description:  子进程处理函数放在此处
 *
 *        Version:  1.0
 *        Created:  12/11/2014 02:00:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include "net_util.h"
#include "util.h"
#include "global.h"
#include "work.h"

int work_init(int i)
{
	work_t *work = &workmgr.works[i];
	//chg title
	chg_proc_title("SimpleServer-%d", work->id);
	//release master resource
	close(epinfo.epfd);
	return 0;
}

int work_dispatch(int i)
{
	return 0;	
}

int work_fini(int i)
{
	return 0;	
}
