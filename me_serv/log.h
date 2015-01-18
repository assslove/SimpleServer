#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <stdio.h>

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef likeyly
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

/* @brief 日志级别
*/
enum LOG_LV {
	LOG_LV_CRIT = 0,
	LOG_LV_ERROR = 1, 
	LOG_LV_INFO = 2, 
	LOG_LV_BOOT = 3,
	LOG_LV_DEBUG = 4, 
	LOG_LV_TRACE = 5,
	LOG_LV_MAX
};

/* @brief 日志输出地址 默认为文件
*/
enum LOG_DEST {
	LOG_DEST_STDOUT = 1, 
	LOG_DEST_FILE = 2, 
	LOG_DEST_FILE_AND_STDOUT = 3
};

/* @brief 日志配置定义
*/
typedef struct log_conf {
	uint32_t maxfiles;		//最大文件个数
	uint32_t per_logsize;	//每个文件大小
	char dirname[128];		//目录名字
	char prename[16];	//日志前缀
	int log_lv;		//日志级别
	int log_dest;		//日志输出
} __attribute__((packed)) log_conf_t;

/* @brief 不同级别的日志fd定义
*/
typedef struct log_fd {
	int fd;		//文件fd
	int day;	//当前日期
	int seq;	//当前日志
	char  basename[64];	//基础名字 如1_debug_20150110
	int baselen;		//基础名字长度 用于查找
} __attribute__((packed)) log_fd_t;

/* @brief init log system
 *
 * @param dirname 目录名字
 * @param lv 日志级别
 * @param filesize 文件最大大小
 * @param maxfiles 最大文件个数
 * @prename 文件名前缀
 */
int log_init(const char *dirname, int lv, uint32_t filesize, uint32_t maxfiles, const char* prename);

/* @brief 日志执行文件
 *
 * @param llv 日志级别
 * @param key 日志主键 一般为用户编号
 */
void do_log(int llv, uint32_t key, const char* fmt, ...);

/* @brief 日志清除
 */
void log_fini();

/* @brief 输出到stdout
 */
void do_std_log(int llv, uint32_t key, const char* fmt, ...);

#define BOOT(key, fmt, arg...)  do_std_log(LOG_LV_BOOT, key, fmt, ##arg)
#define TRACE(key, fmt, arg...) do_log(LOG_LV_TRACE, key, fmt, ##arg)
#define DEBUG(key, fmt, arg...) do_log(LOG_LV_DEBUG, key, fmt, ##arg)
#define INFO(key, fmt, arg...) do_log(LOG_LV_INFO, key, fmt, ##arg)
#define ERROR(key, fmt, arg...) do_log(LOG_LV_ERROR, key, fmt, ##arg)
#define CRIT(key, fmt, arg...) do_log(LOG_LV_CRIT, key, fmt, ##arg)

#endif
