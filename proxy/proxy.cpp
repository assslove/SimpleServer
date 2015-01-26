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

#include <libnanc/proto_head.h>
}

#include <libnanc++/util.h>

#include "proxy.h"
#include "router.h"

void  Proxy::handleRequest(int fd, proto_pkg_t *pkg)
{
#ifdef SERV_ONE
	temp_user_id = pkg->id;
#else
	temp_user_id = pkg->id, temp_user_id = temp_user_id << 32 | pkg->svr_id;
#endif
	g_proxy.save(temp_user_id, fd);
	if (g_proxy.doRouter(pkg)) { //处理失败
		g_proxy.del(temp_user_id);
	}

	//return send_to_cli(get_fd(pkg->seq), cli, pkg->len);
}

void Proxy::handleResponse(const proto_pkg_t *pkg)
{
#ifdef SERV_ONE
	temp_user_id = pkg->id;
#else
	temp_user_id = pkg->id, temp_user_id = temp_user_id << 32 | pkg->svr_id;
#endif
	int m_tempfd = get(temp_user_id);
	if (m_tempfd == 0) {
		ERROR(0, "cannot find callback fd [id=%u]", pkg->id);
		return ;
	}

	send_to_cli(get_fd(m_tempfd), pkg, pkg->len);
}

void Proxy::handleCliClosed(int fd)
{
	std::vector<int> remove_list;	
	FOREACH (it, fds) {
		if (it->second == fd) {
			remove_list.push_back(it->first);
		}
	}

	FOREACH (it, remove_list) {
		fds.erase(*it);
	}
}

void Proxy::handleServClosed(int fd)
{
	
}
