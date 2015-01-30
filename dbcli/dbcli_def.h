#ifndef _DBCLI_DEF_H
#define _DBCLI_DEF_H

/* @brief 定义错误码与宏
 */

/* @brief 错误码内部定义 1-1000 预留，应用程序不可使用
 */
enum DB_ERROR_CODE {
	DB_SUCCUSS = 0,			//成功	
	DB_ERROR = 1,			//统一错误
	DB_ERR_STORE_RES = 1,	//存储结果失败
}
#endif
