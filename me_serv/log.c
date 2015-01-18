/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  log System 
 *    日志轮转，按级别统计日志，非线程安全
 *	  日志文件命名  X_LV_YYYYMMDD_XXXXXX(1_debug_20150110_000001)
 *        Version:  1.0
 *        Created:  2014年11月24日 21时43分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou , houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "log.h"

#define FILENAM_LEN 128
static log_fd_t logfds[LOG_LV_MAX];
static log_conf_t logconf;
static const char *LOG_LV_NAME[LOG_LV_MAX] = {
	"crit", 
	"error", 
	"info", 
	"debug", 
	"trace", 
	"boot"
};

static int gen_log_seq(int llv, struct tm *tm) 
{
	DIR *dir = opendir(logconf.dirname);
	if (!dir) {
		fprintf(stderr, "open dir %s failed [%s]", logconf.dirname, strerror(errno));
		return -1;
	}

	struct dirent *file;
	int init_seq = logfds[llv].seq;
	int tmp_seq = 0;
	char seq_str[10] = {'\0'};
	while ((file = readdir(dir))) { //找出文件相等比较大的序号
		if (strncmp(file->d_name, logfds[llv].basename, logfds[llv].baselen) == 0) {
			sprintf(seq_str, "%s", file->d_name + logfds[llv].baselen);
			tmp_seq = atoi(seq_str);
			if (init_seq < tmp_seq) {
				init_seq = tmp_seq;
			}
		}
	}

	int ret = -1;
	if (unlikely(init_seq >= logconf.maxfiles)) { //大于最大文件 不写了
		fprintf(stderr, "file exceed max");
		logfds[llv].seq = -1;
	} else {
		if (logfds[llv].fd) { //回收
			close(logfds[llv].fd);
		}
		logfds[llv].seq = (logfds[llv].seq + 1) % logconf.maxfiles;
		ret = 0;
	}

	closedir(dir);
	return ret;
}

int log_init(const char *dirname, int lv, uint32_t filesize, uint32_t maxfiles, const char* prename)
{
	int ret = -1;
	assert(lv >= LOG_LV_BOOT && lv < LOG_LV_MAX);
	assert(maxfiles >= 1 && maxfiles <= 100000);
	assert(filesize >= 10 && filesize <= 100 * 1024 * 1024); //10 - 100M

	logconf.maxfiles = maxfiles;
	logconf.per_logsize = filesize;
	logconf.log_lv = lv;
	logconf.log_dest = LOG_DEST_FILE;

	//检测输入条件
	if (!dirname || strlen(dirname) == 0) {
		return ret;
	}

	if (access(dirname, W_OK)) { //test write
		fprintf(stderr, "access %s failed\n", dirname);
		return ret;
	}

	memset(logconf.dirname, 0, sizeof(logconf.dirname));
	strcpy(logconf.dirname, dirname);

	if (!prename) {
		fprintf(stderr, "prename is null");
		return -1;
	}

	memset(logconf.prename, 0, sizeof(logconf.prename));
	strcpy(logconf.prename, prename);

	time_t cur = time(0);
	struct tm tm;
	localtime_r(&cur, &tm);
	int i;
	char day_str[16] = {'\0'};
	sprintf(day_str, "%04d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	for (i = LOG_LV_CRIT; i < LOG_LV_MAX; ++i) {
		logfds[i].baselen = snprintf(logfds[i].basename, 64, "%s_%s_%s_", logconf.prename, LOG_LV_NAME[i], day_str);
		logfds[i].fd = -1;
		logfds[i].seq = 1;
		logfds[i].seq = gen_log_seq(i, &tm);
		logfds[i].day = tm.tm_mday;
	}

	ret = 0;

	return ret;
}


void log_fini()
{
	int i = 0;
	for (i = LOG_LV_BOOT; i < LOG_LV_MAX; ++i) {
		if (logfds[i].fd != -1) {
			close(logfds[i].fd);
		}
	}
}

/* @brief 打开当前级别的日志文件
*/
static int open_file(int llv, struct tm *tm) 
{
	static char filename[256] = {'\0'};
	int need_open = 0;
	if (unlikely(logfds[llv].fd == -1)) { //打开文件
		if (gen_log_seq(llv, tm) == -1) {
			return -1;
		}
		need_open = 1;
		sprintf(filename, "%s/%s%06d", logconf.dirname, logfds[llv].basename, logfds[llv].seq);
	} else { //有文件 判断是不是同一天 需要更换文件
		if (tm->tm_mday != logfds[llv].day) { //记录
			logfds[llv].day = tm->tm_mday;	
			char day_str[16] = {'\0'};
			sprintf(day_str, "%04d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
			snprintf(logfds[llv].basename, 64, "%s_%s_%s", logconf.prename, LOG_LV_NAME[llv], day_str);
			sprintf(filename, "%s/%s%06d", logconf.dirname, logfds[llv].basename, logfds[llv].seq);
			need_open = 1;
		} else if (lseek(logfds[llv].fd, 0, SEEK_END) >= logconf.per_logsize) {
			if (gen_log_seq(llv, tm) == -1) {
				return -1;
			}
			sprintf(filename, "%s/%s%06d", logconf.dirname, logfds[llv].basename, logfds[llv].seq);
			need_open = 1;
		} 
	}

	if (need_open) {
		logfds[llv].fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0644);
		if (logfds[llv].fd == -1) {
			fprintf(stderr, "open %s failed [%s]", filename, strerror(errno));
			return -1;
		}
	}

	return 0;
}

void do_log(int llv, uint32_t key, const char* fmt, ...)
{
	if (llv >= logconf.log_lv) { //不用记录
		return ;
	}

	time_t now = time(NULL);	
	struct tm tm;
	localtime_r(&now, &tm);

	if (open_file(llv, &tm) == -1) {
		return ;
	}

	va_list ap;
	va_start(ap, fmt);
#define LOGBUF_LEN 2048
	static char logbuf[LOGBUF_LEN] = {'\0'};
	static int start = 0;
	static int end = 0;
	start = snprintf(logbuf, LOGBUF_LEN, "%02u:%02u:%02u %u ", tm.tm_hour, tm.tm_min, tm.tm_sec, key);
	end = vsnprintf(logbuf + start, LOGBUF_LEN - start, fmt, ap);
	va_end(ap);

	if (unlikely(start + end >= LOGBUF_LEN)) {
		end = LOGBUF_LEN - start;
		logbuf[LOGBUF_LEN - 1] = '\n';
	} else {
		logbuf[start + end] = '\n';
	}

	write(logfds[llv].fd, logbuf, start + end + 1);
}

void do_std_log(int llv, uint32_t key, const char* fmt, ...)
{
	time_t now = time(NULL);
	struct tm tm;
	localtime_r(&now, &tm);

	va_list ap;
	va_start(ap, fmt);
	fprintf(stdout, "[%02u:%02u:%02u] %u ", tm.tm_hour, tm.tm_min, tm.tm_sec, key);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);
	va_end(ap);
}
