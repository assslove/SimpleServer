/*
 * =====================================================================================
 *
 *       Filename:  db_name.cpp
 *
 *    Description:  对库名表名的封装, 针对分表分库用
 *
 *        Version:  1.0
 *        Created:  2015年01月27日 23时16分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  XiaoNan, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <string.h>
#include <mysqld_error.h>

#include "mysql_cli.h"
#include "dbcli_def.h"
#include "table.h"

Table::Table(MysqlCli *mc_, const char* dbname_, const char* tbname)
{
	this->m_mc = mc_;	
	strcpy(m_dbname, dbname_);
	strcpy(m_tbname, tbname);
}

int Table::execUpdateSql(const char* sqlstr_, int len_, int *affectrows_)
{
	return this->m_mc->mysqlExecUpdate(sqlstr_, len_, affectrows_);
}

int Table::execInsertSql(const char* sqlstr_, int len_, int duplicate_)
{
	int affectrows, ret;
	this->m_mc->mysqlSetId(m_id);
	if ((ret = this->m_mc->mysqlExecUpdate(sqlstr_, len_, &affectrows)) == 0) {
		return 0;
	} else if (ret == ER_DUP_ENTRY) { //主键重复返回错误码
		return duplicate_;
	} 

	return ret;
}

int Table::execDeleteSql(const char* sqlstr_, int len_, int nofind_)
{
	int affectrows, ret;
	this->m_mc->mysqlSetId(m_id);
	if ((ret = this->m_mc->mysqlExecUpdate(sqlstr_, len_, &affectrows)) == 0) {
		if (affectrows == 0) { //没有删除指定的行 说明找不到
			return nofind_;
		} else {
			return 0;
		}
	} 

	return ret;
}

int Table::execInsertSqlAndGetLastId(const char* sqlstr_, int len_, int duplicate_, uint32_t *lastid_)
{
	int affectrows, ret;	
	this->m_mc->mysqlSetId(m_id);
	if ((ret = this->m_mc->mysqlExecUpdate(sqlstr_, len_, &affectrows)) == 0) {
		*lastid_ = this->m_mc->mysqlGetLastId();
		return 0;
	} 

	return ret;
}

char* Table::getTableName()
{
	this->m_mc->mysqlSelectDB(this->m_dbname);	
	sprintf(this->m_db_tb_name, "%s.%s", this->m_dbname, this->m_tbname);
	return this->m_db_tb_name;
}

int Table::execQuerySql(MYSQL_RES **res_, uint32_t *count_) 
{
	int ret;
	this->m_mc->mysqlSetId(m_id);
	if ((ret = this->m_mc->mysqlExecQuery(m_sqlstr, m_sqllen, res_))) {
		return ret;
	} else {
		*count_ = this->m_mc->mysqlGetAffectedRows(*res_);
		return 0;
	}
}	
