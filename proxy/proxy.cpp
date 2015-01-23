/*
 * =====================================================================================
 *
 *       Filename:  switch.cpp
 *
 *    Description:  switch 测试
 *
 *        Version:  1.0
 *        Created:  2014年12月30日 15时36分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <string.h>

extern "C" {
#include <sserv/log.h>
#include <sserv/net_util.h>
#include <sserv/conf.h>
#include <sserv/fds.h>
}

#include "proxy.h"
#include "router.h"

int Proxy::handleRequest(int fd, proto_pkg_t *pkg)
{
	//DEBUG(pkg->id, "switch callback len=%u,id=%u,seq=%u,cmd=%u,ret=%u, msg=%s", pkg->len, pkg->id, pkg->seq, pkg->cmd, pkg->ret, (char *)pkg->data);

	//处理多服与单服包头的不一样
#ifdef SERV_ONE
	temp_user_id = pkg->id;
#else
	temp_user_id = pkg->id, temp_user_id = temp_user_id << 32 | pkg->svr_id;
#endif
	proxy->save(temp_user_id, fd);
	if (proxy->doRouter(pkg)) { //处理失败
		proxy.del(temp_user_id);
	}

	//return send_to_cli(get_fd(pkg->seq), cli, pkg->len);
}

int Proxy::handleResponse(int fd, void *msg, int len)
{
	
}
