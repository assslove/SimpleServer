/*
 * =====================================================================================
 *
 *       Filename:  test_serv.cpp
 *
 *    Description:  proxy服务器 用于数据库与其它平台服务的交互
 *
 *        Version:  1.0
 *        Created:  12/23/2014 05:18:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <sserv/log.h>
#include <sserv/net_util.h>
#include <sserv/conf.h>
#include <sserv/fds.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus 
#define OUTER_FUNC extern "C"
#else
#define OUTER_FUNC 
#endif 

#include "proxy.h"

OUTER_FUNC void handle_timer()
{
}

OUTER_FUNC int proc_cli_msg(void *msg, int len, fdsess_t *sess)
{
	//校验
	proto_pkg_t *pkg = reinterpret_cast<proto_pkg_t *>(msg);

	DEBUG(pkg->id, "proxy len=%u,id=%u,seq=%u,cmd=%u,ret=%u, msg=%s", pkg->len, pkg->id, pkg->seq, pkg->cmd, pkg->ret, (char *)pkg->data);

	//pkg->seq = sess->fd;
	//uint32_t  cli[1024];
	//memcpy(cli, msg, pkg->len);

	//if (switch_fd == -1) {
		//switch_fd = connect_to_serv(conf_get_str("switch_ip"), conf_get_int("switch_port"), 1024, 1000); 
	//}

	//if (switch_fd == -1) {
		//ERROR(0, "cannot connect to switch");
	//}
	g_proxy.handleRequest(sess->fd, pkg);
	return 0;
//	return send_to_cli(sess, cli, pkg->len);
	//return send_to_serv(switch_fd, cli, pkg->len);
}

OUTER_FUNC int proc_serv_msg(int fd, void *msg, int len)
{
	g_proxy.handleResponse(fd, msg, len);
	return 0;
}

OUTER_FUNC int on_cli_closed(int fd) 
{
	INFO(0, "cli fd=%u closed", fd);
	return 0;
}

OUTER_FUNC int on_serv_closed(int fd)
{
	//TODO 找个fd，并把fd置为-1
	//if (fd == switch_fd) {
		//switch_fd  = -1;
		//ERROR(0, "switch fd closed [fd=%d]", fd);
	//}
	
	ERROR(0, "fd closed [fd=%d]", fd);

	return 0;
}

OUTER_FUNC int serv_init(int ismaster) 
{
	if (g_router.loadRouterXml()) {
		ERROR(0, "load router.xml failed");
		return -1;
	}
	//打印router信息
	g_router.printRouter();

	INFO(0, "%s init", ismaster ? "master" : "work");

	//if (switch_fd == -1) {
		//switch_fd = connect_to_serv(conf_get_str("switch_ip"), conf_get_int("switch_port"), 1024, 1000); 
	//}

	//if (switch_fd == -1) {
		//ERROR(0, "cannot connect to switch");
	//}
	
	return 0;
}

OUTER_FUNC int serv_fini(int ismaster) 
{
	INFO(0, "%s fini", ismaster ? "master" : "work");
	return 0;
}

OUTER_FUNC int	get_msg_len(int fd, const void *data, int len, int ismaster)
{
	return *(int *)(data);
}


