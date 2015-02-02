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
#include <dbcli/dbcli_def.h>

#include "user.h"
//#include "proto/lib/user.pb.h"

User::User(MysqlCli *mc_) : TableRouter(mc_, "TEST", "t_user", 1, 1)
{
	
}

int User::getUser()
{
	GENSQL("select * from t_user where user_id=%u", 1000);		
	QUERY_ONE_BEGIN(1) 
		DEBUG(0, "user_id=%u", INT_NEXT_COL);	
		DEBUG(0, "name=%s", NEXT_COL);	
		DEBUG(0, "score=%u", INT_NEXT_COL);	
	QUERY_ONE_END()
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
