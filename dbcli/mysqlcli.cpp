/*
 * =====================================================================================
 *
 *       Filename:  mysqlcli.cpp
 *
 *    Description:  对mysqlcli的测试 用于测试性能 图表显示 
 *
 *        Version:  1.0
 *        Created:  2015年01月07日 13时40分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mysql.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[]) 
{
	if (mysql_library_init(0, NULL, NULL)) {
		fprintf(stderr, "could not init MySQL lib");
		exit(1);
	}
	MYSQL *mysql = mysql_init(NULL);
	mysql = mysql_real_connect(mysql, "127.0.0.1", "root", "8459328", NULL, 3306, NULL, 0);
	if (!mysql) {
		fprintf(stderr, "cannot not connect mysql");
		exit(1);
	}
	mysql_select_db(mysql, "TEST");
	mysql_autocommit(mysql, 0);
	mysql_commit(mysql);
	mysql_autocommit(mysql, 1);
	char from[128] = {"select * from t_user"};
	char to[1024];
	uint32_t len = mysql_real_escape_string(mysql, to, from, strlen(from));
	printf("len = %u\n from =%u", len, strlen(from));
	mysql_real_query(mysql, to, 1024);
	MYSQL_RES *res = mysql_store_result(mysql);
	mysql_close(mysql);
	mysql_library_end();
	return 0;
}
