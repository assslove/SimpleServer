/*
 * =====================================================================================
 *
 *       Filename:  router.cpp
 *
 *    Description:  路由信息解析配置表
 *
 *        Version:  1.0
 *        Created:  2015年01月22日 14时22分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */
extern "C" {
#include <libnanc/log.h>
}

#include <libnanc++/util.h>

#include "router.h"

void RouterManager::printRouter()
{
	RouterIter it = routers.begin();	
	for (; it != routers.end(); ++it) {
		INFO(0, "router [id=%u][type=%u]", it->first, it->second.type);	
		it->second.databases.print();
	}
}

void RouterManager::preProcess()
{
	RouterIter it = routers.begin();	
	for (; it != routers.end(); ++it) {
		it->second.databases.sort_less();
		FOREACH(i, it->second.databases.get_range_vec()) { //对tables进行排序
			i->sort();
		}
	}
}

const table_t* RouterManager::getRouter(uint16_t cmd, uint32_t id)
{
	router_t *prouter = NULL;
	it = routers.find(cmd);
	if (it != routers.end()) {
		prouter = &it->second;	
	}

	uint32_t db_idx = id % 100;
	return NULL;	
}

int RouterManager::sendAcrossRouter(proto_pkg_t *pkg)
{
	const table_t *tb = getRouter(pkg->cmd, pkg->id);	
}

const table_t* RouterManager::getRouterByFd(int fd)
{
	return NULL;	
}
