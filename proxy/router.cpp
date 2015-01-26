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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C" {
#include <libnanc/log.h>
#include <sserv/log.h>
#include <sserv/net_util.h>
#include <sserv/conf.h>
#include <sserv/fds.h>
}

#include <libnanc++/util.h>
#include <libnanc++/xml_parser.h>

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
		FOREACH (i, it->second.databases.get_range_vec()) { //对tables进行排序
			i->sort();
		}
	}
}

const table_t* RouterManager::getRouter(uint16_t cmd, uint32_t id)
{
	cmd = cmd >> 8; //高8位代表类型 
	it = routers.find(cmd);
	if (it == routers.end()) {
		return NULL;
	}

	m_router = &it->second;
	
	m_database = m_router->databases.find(id % 100);
	if (!m_database) {
		ERROR(0, "router config error [router_id=%u]", it->first);
		return NULL;
	}
		
	return m_database->tables.find(get_table_idx(m_router->type, id));	
}

int RouterManager::sendAcrossRouter(proto_pkg_t *pkg)
{
	const table_t *tb = getRouter(pkg->cmd, pkg->id);	
	if (!tb) {
		ERROR(0, "cannot find router [cmd=%u][id=%u]", pkg->cmd, pkg->id);
		return 0;
	}

	if (tb->fd == -1) {
		std::string remote_ip = inet_ntoa(*((struct in_addr*)&remote_ip));
		tb->fd = connect_to_serv(remote_ip.c_str(), tb->remote_port, 10240, 1000);
		if (tb->fd == -1) {
			ERROR(0, "cannot connect to serv [remote_ip=%s], [remote_ip=%u]", remote_ip.c_str(), tb->remote_port);
			return -1;
		}
	}

	return send_to_serv(tb->fd, pkg, pkg->len);
}

const table_t* RouterManager::getRouterByFd(int fd)
{
	return NULL;	
}

int RouterManager::loadRouterXml()
{
	routers.clear();

	INFO(0, "load router xml begin");
	XmlParser parser;
	if (!parser.initFile("conf/router.xml")) {
		ERROR(0, "load router.xml failed");
		return -1;
	}

	xmlNodePtr root = parser.getRootNode("Routers"); 	
	if (root) {
		xmlNodePtr ch1 = parser.getChildNode(root, "Router");
		while (ch1) {
			router_t item;
			uint32_t cmd = 0;
			parser.getNodePropNum(ch1, "routeId", &cmd, sizeof(cmd));
			parser.getNodePropNum(ch1, "type", &item.type, sizeof(item.type));

			xmlNodePtr ch2 = parser.getChildNode(ch1, "Database");
			while (ch2) {
				database_t db;
				parser.getNodePropNum(ch2, "idStart", &db.start, sizeof(db.start));
				parser.getNodePropNum(ch2, "idEnd", &db.end, sizeof(db.end));


				xmlNodePtr ch3 = parser.getChildNode(ch2, "Table");

				while(ch3) {
					table_t tb;
					parser.getNodePropNum(ch3, "subIdStart", &tb.start, sizeof(tb.start));
					std::string remote_ip;	
					struct in_addr temp_addr;
					parser.getNodePropStr(ch3, "remoteIp", remote_ip);
					if (inet_aton(remote_ip.c_str(), &temp_addr) == -1) {
						ERROR(0, "ip format invalid [%s]", remote_ip.c_str());
						return -1;
					}
					tb.remote_ip = temp_addr.s_addr;

					parser.getNodePropNum(ch3, "subIdEnd", &tb.end, sizeof(tb.end));
					parser.getNodePropNum(ch3, "remotePort", &tb.remote_port, sizeof(tb.remote_port));
					tb.fd = connect_to_serv(remote_ip.c_str(), tb.remote_port, 10240, 1000);
					if (tb.fd == -1) {
						 ERROR(0, "cannot connect to serv [remote_ip=%s], [remote_ip=%u]", remote_ip.c_str(), tb.remote_port);
					}
					db.tables.push_back(tb);

					ch3	= parser.getNextNode(ch3, "Table");
				}
				item.databases.push_back(db);
				ch2	= parser.getNextNode(ch2, "Database");
			}
			routers.insert(std::make_pair<uint32_t, router_t>(cmd, item));
			ch1 = parser.getNextNode(ch1, "Router");
		}
	}

	INFO(0, "load router xml end [%u]", routers.size());
	preProcess();
	INFO(0, "sort router xml end");
	return 0;	
}

