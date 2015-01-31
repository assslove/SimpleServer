#ifndef _DBCLI_DEF_H
#define _DBCLI_DEF_H

/* @brief 错误码内部定义 1-1000 预留，应用程序不可使用
 */
enum DB_ERROR_CODE {
	DB_SUCC = 0,			//成功	
	DB_ERR = 1,				//统一错误
	DB_ERR_STORE_RES = 2,	//存储结果失败
};

inline safe_atoi(const char *str) 
{
	return (str != NULL) ? atoi(str) : 0;
}

inline safe_atol(const char *str) 
{
	return (str != NULL) ? atoll(str) : 0;
}

#endif
