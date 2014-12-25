#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <stdio.h>

/* @brief log lv
 */
enum LOG_LV {
	LOG_LV_CRIT = 0,
	LOG_LV_ERROR = 1, 
	LOG_LV_INFO = 2, 
	LOG_LV_DEBUG = 3, 
	LOG_LV_TRACE = 4,
	LOG_LV_BOOT = 5
};

void slog(int llv, uint32_t key, const char* fmt, ...);

#define BOOT(key, fmt, arg...) printf("%d "fmt, key, ##arg)
#define TRACE(key, fmt, arg...) slog(LOG_LV_TRACE, key, fmt, ##arg)
#define DEBUG(key, fmt, arg...) slog(LOG_LV_DEBUG, key, fmt, ##arg)
#define INFO(key, fmt, arg...) slog(LOG_LV_INFO, key, fmt, ##arg)
#define ERROR(key, fmt, arg...) slog(LOG_LV_ERROR, key, fmt, ##arg)
#define CRIT(key, fmt, arg...) slog(LOG_LV_CRIT, key, fmt, ##arg)

#endif
