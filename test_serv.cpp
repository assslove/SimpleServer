/*
 * =====================================================================================
 *
 *       Filename:  test_serv.cpp
 *
 *    Description:  对SimpleServer 的测试 echo 服务器
 *
 *        Version:  1.0
 *        Created:  12/23/2014 05:18:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */


#include <iostream>
#include "fds.h"
#include "outer.h"

/* @brief 定义包
*/
typedef struct proto_pkg {
	int len;
	int uid;
	int seq;
	int cmd;
	int ret;
} __attribute__((packed))proto_pkg_t;

extern "C" void handle_timers()
{

}

extern "C" int proc_cli_msg(void *msg, int len, fdsess_t *sess)
{
	return 0;
}

extern "C" int proc_serv_msg(int fd, void *msg, int len)
{
	return 0;
}

extern "C" int on_cli_closed(int fd) 
{
	return 0;
}

extern "C" int on_serv_closed(int fd)
{
	return 0;
}

extern "C" int serv_init(int ismaster) 
{
	return 0;
}

extern "C" int serv_fini(int master) 
{
	return 0;
}

extern "C" int	get_msg_len(int fd, const void *data, int len, int ismaster)
{
	return 0;
}
