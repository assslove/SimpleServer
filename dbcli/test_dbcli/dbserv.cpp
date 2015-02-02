/*
 * =====================================================================================
 *
 *       Filename:  dbserv.cpp
 *
 *    Description:  用来测试dbcli的各种功能
 *
 *        Version:  1.0
 *        Created:  2015年02月02日 11时14分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <dbcli/mysql_cli.h>
#include <dbcli/table_router.h>
#include <dbcli/table.h>

#include "dispatch.h"
#include "user.h"
#include "limit.h"

int main(int argc, char* argv[]) 
{
	if (log_init("log", LOG_LV_TRACE, 10240, 1000, "db") == -1) {
		printf("log init error\n");
		return 1;
	}
	
	MysqlCli *mc = new MysqlCli("localhost", "root", "8459328", 3306, "utf8");
	int ret = mc->mysqlInit();
	if (ret == -1) { //连接错误
		return 1;
	}

	DEBUG(0, "db start success");

	Dispatch *dispatch = new Dispatch(mc);

	dispatch->m_user->insertUser();
	dispatch->m_user->getUser();
	dispatch->m_user->updateUser();
	dispatch->m_user->deleteUser();

	delete dispatch;
	dispatch = NULL;

	log_fini();

	return 0;
}
