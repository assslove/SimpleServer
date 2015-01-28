#ifndef _MYSQL_CLI_H
#define _MYSQL_CLI_H

#include <mysql.h>

/* @brief 封装mysql常用函数
 */
class MysqlCli {

	public:
		MysqlCli();
		~MysqlCli();

		int mysqlInit();		
		
	private:
		char m_host[16];
		char m_user[16];
		char m_passwd[32];
		MYSQL *m_mysql;    //mysql实例 
};

#endif
