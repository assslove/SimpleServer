/*
 * =====================================================================================
 *
 *       Filename:  limit.cpp
 *
 *    Description:  limit 数据库操作
 *
 *        Version:  1.0
 *        Created:  2015年02月02日 16时50分00秒
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

#include "limit.h"

Limit::Limit(MysqlCli *mc_) : TableRouter(mc_, "TEST", "t_limit", 1, 1)
{
	
}
