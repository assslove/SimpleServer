#ifndef _MYSQL_CLI_H
#define _MYSQL_CLI_H

#include <mysql.h>

extern "C" {
#include <libnanc/log.h>
}

/* @brief 封装mysql常用函数
 */
class MysqlCli {

	public:
		MysqlCli(const char *host_, const char *user_, const char *passwd_, uint16_t port, const char *charset_);
		~MysqlCli();

		/* @brief 初始化Mysql
		 * @return 0 on success, -1 on error
		 */
		int mysqlInit();		
		
		/* @brief 连接mysql,以防数据库重启后可以访问
		 */
		int mysqlConnect();

		/* @brief 执行查询
		 */
		int mysqlExecQuery(const char *sqlstr_, int sqllen_, MYSQL_RES **res_);

		/* @brief 执行更新
		 */
		int mysqlExecUpdate(const char *sqlstr_, int sqllen_, int* affectRows_);

		/* @brief 设置自动提交
		 * @param ison 1-打开 0-关闭
		 */
		int mysqlAutocommit(int ison_) {
			if ((m_ret = mysql_autocommit(m_mysql, ison_))) {
				ERROR(0, "mysql autocommit set failed [%s]", mysql_error(m_mysql));
				return m_ret;
			}
		}

		/* @brief 选择数据库
		 */
		int mysqlSelectDB(const char* dbname_) {
			if ((m_ret = mysql_select_db(m_mysql, dbname_))) {
				ERROR(0, "mysql select db err [%s]", mysql_error(m_mysql));
			}

			return m_ret;
		}

	private:

		/* @brief 执行sql语句
		 */
		int mysqlExec(const char *sqlstr_, int sqllen_);

	private:
		char m_host[16];    //主机
		char m_user[16];    //用户名
		char m_passwd[32];  //密码
		uint16_t m_port;	//端口号
		char m_charset[16]; //字符集
		MYSQL* m_mysql;     //mysql实例 
		int m_ret;			//返回值
};

#endif
