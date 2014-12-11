#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "net_util.h"
#include "master.h"

/* 定义的一些常量
 */
enum CONST_DEF{
	MAX_WORKS = 1024, 
};

/* @brief 返回类型
 */
enum RTYPE {
	SUCCESS = 0,
	ERROR = -1 
};

extern int g_argc;
extern char **g_argv;
extern char *argv_start;
extern char *argv_end;
extern char *env_end;

extern int chl_pids[MAX_WORKS];
extern work_mgr_t workmgr;
extern epoll_info_t epinfo;
extern svr_setting_t setting;

#endif
