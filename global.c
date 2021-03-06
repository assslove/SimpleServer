/*
 * =====================================================================================
 *
 *       Filename:  global.c
 *
 *    Description:  全局变量定义区 
 *
 *        Version:  1.0
 *        Created:  12/07/2014 09:33:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */


#include "global.h"
#include "fds.h"
#include "net_util.h"
#include "master.h"
#include "list.h"
#include "outer.h"


/* @brief 用于修改进进程名字用
 */
int g_argc;
char **g_argv;
char *argv_start;
char *argv_end;
char *env_end;

int chl_pids[1024] = {0};

//work进程管理
work_mgr_t  workmgr;
//epoll相关信息
epoll_info_t epinfo;
//配置信息
svr_setting_t setting = {
	10240,
	1024, 
	1024 * 1024 * 10,  //10M
	32 * 1024,		   //32k
	1024 * 1024 * 10,  //10M
	"SimpleServer"
};
//配置文件
GHashTable *sim_data;
//fd manager
GHashTable *fds;
//stop flag
int stop;
//log filename
char log_file[64] = "log/temp.log";
