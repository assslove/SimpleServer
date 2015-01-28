#ifndef _MYSQL_CLI_H
#define _MYSQL_CLI_H

#include <mysql.h>

/* @brief 封装mysql常用函数
 */
class MysqlCli {

	public:
		MysqlCli(const char *host_, const char *user_, const char *passwd_, uint16_t port);
		~MysqlCli();

		int mysqlInit();		
		
	private:
		char m_host[16];    //主机
		char m_user[16];    //用户名
		char m_passwd[32];  //密码
		uint16_t m_port;	//端口号
		MYSQL* m_mysql;     //mysql实例 
};

#endif
