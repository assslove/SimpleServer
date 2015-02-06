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
#include "proto/lib/user.pb.h"

using namespace db_proto;

User::User(MysqlCli *mc_) : TableRouter(mc_, "TEST", "t_user", 1, 1)
{
	
}

int User::getUser()
{
	GENSQL("select * from %s where user_id=%u", this->getTableName(1000), 2000);
	DEBUG(0, "%s, %u", this->m_sqlstr, this->m_sqllen);
	MUser user;
	QUERY_ONE_BEGIN(1) 
		user.set_userid(INT_NEXT_COL);
		user.set_name(NEXT_COL);
		user.set_score(INT_NEXT_COL);

		DEBUG(0, "%u,%s,%u", user.userid(), user.name().c_str(), user.score());
	QUERY_ONE_END()
}

void User::insertUser()
{
	char name[1024];
	MYSQL_REAL_ESCAPE_STRING(name, "小小鸟", strlen("小小鸟"));
	GENSQL("insert into %s values(%u, '%s', %u)", this->getTableName(3000), 3000, name, 20);
	if (int ret = this->execInsertSql(1)) {
		ERROR(0, "cannot insert %u", ret);
	}
}

void User::updateUser()
{
	GENSQL("update %s set score=%u", this->getTableName(2000), 1);
	if (int ret = this->execUpdateSql()) {
		ERROR(0, "cannot update %u", ret);
	}
}

void User::deleteUser()
{
	GENSQL("delete from %s where user_id=%u", this->getTableName(2000), 2000);
	if (int ret = this->execDeleteSql(2)) {
		ERROR(0, "cannot update %u", ret);
	}
}
