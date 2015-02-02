#ifndef _DBCLI_DEF_H
#define _DBCLI_DEF_H

#include <stdlib.h>
#include <stdint.h>
#include <mysql.h>

/* @brief 错误码内部定义 1-1000 预留，应用程序不可使用
 */
enum DB_ERROR_CODE {
	DB_SUCC = 0,			//成功	
	DB_ERR = 1,				//统一错误
	DB_ERR_STORE_RES = 2,	//存储结果失败
};

/* @brief str to int
 */
inline int safe_atoi(const char *str) 
{
	return (str != NULL) ? atoi(str) : 0;
}

/* @brief str to uint64_t
 */
inline uint64_t safe_atol(const char *str) 
{
	return (str != NULL) ? atoll(str) : 0;
}

//开始单个查询开始
#define QUERY_ONE_BEGIN(nofind_err) { \
	MYSQL_RES *res; \
	MYSQL_ROW row;\
	int count, ret;\
	ret = this->execQuerySql(&res, &count);\
	if (ret == 0) {\
		if (count != 1) {\
			mysql_free_result(res);\
			DEBUG(0, "no record exist [%u]", nofind_err);\
			return no_finderr;\
		} else { \
			row = mysql_fetch_row(res);	\
			int idx_ = -1;\

//开始单个查询结束
#define QUERY_ONE_END() \
			mysql_free_result(res); \
			return 0;\
		}\
	} else {\
		return DB_ERR; \
	}

//下一行
#define  NEXT_COL	(row[++idx_])

//cpy int 
#define INT_NEXT_COL(val)  (val)=safe_atoi(NEXT_COL)

//cpy int64
#define INT64_NEXT_COL(val)  (val)=safe_atol(NEXT_COL)

//拷贝字符串 二进制
#define STR_NEXT_COL(val, max) \
	++idx_;\
	mysql_fetch_lengths(res); \
	if (res->lengths[idx_] < max) {\
		memcpy(val, row[idx_], res->lengths[idx_]);\
	} else {\
		memcpy(val, row[idx_], max);\
	}
	
//开始循环接收数据 protobuf
//type指类型 msg 指protobuf msg, field指msg中的项 指带结构体的类型，
//如果是普通类型见QUERY_MULTI_NORMAL_BEGIN
#define QUERY_MULTI_BEGIN()  \
	MYSQL_RES *res;\
	MYSQL_ROW row;\
	int count, ret;\
	ret = this->execQuerySql(&res, &count);\
	if (ret == 0) {\
		while (row = mysql_fetch_row(res)) {\
			int idx_ = -1;

//结束循环接收数据
#define QUERY_MULTI_END()\
		}\
		mysql_free_result(res);\
		return 0;\
	} else {\
		return DB_ERR;\
	}

#endif
