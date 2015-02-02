/*
 * =====================================================================================
 *
 *       Filename:  用户表
 *
 *    Description:  用来测试对数据库的访问
 *
 *        Version:  1.0
 *        Created:  2015年02月02日 11时32分36秒
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

#include "user.h"
#include "proto/lib/user.pb.h"

User::User(MysqlCli *mc_) :
	TableRouter(mc_, "TEST", "t_user", 1, 1)
{
	
}

void User::getUser()
{
	
}

void User::insertUser()
{
	
}

void User::updateUser()
{
	
}

void User::deleteUser()
{
	
}
