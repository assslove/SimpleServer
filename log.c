/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  log System
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "log.h"

extern char log_file[64];

void slog(int llv, uint32_t key, const char* fmt, ...)
{
	time_t now = time(NULL);
	struct tm tm;
	localtime_r(&now, &tm);
	
	
	va_list ap;
//	strcat(logfile, LOG_LV_NAME[llv]);

	FILE* file = fopen(log_file, "a+");
	va_start(ap, fmt);
	fprintf(file, "[%02u:%02u:%02u] %u ", tm.tm_hour, tm.tm_min, tm.tm_sec, key);
	vfprintf(file, fmt, ap);
	fprintf(file, "\n");
	fflush(file);

	va_end(ap);
	fclose(file);
}
