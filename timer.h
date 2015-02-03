#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>
#include <time.h>

#include "min_heap.h"
#include "log.h"

extern timeval now; //保存当前时间戳
extern tm cur_tm; //保存当前tm结构

inline void refresh_time()
{
	gettimeofday(&now, 0);
	localtime_r(&now.tv_sec, &cur_tm);
}

/* @brief 处理定时事件
 */
inline void timer_handle()
{
	refresh_time();

	static time_t last; //最后一次时间
	if (last != now.tv_sec) {
		last = now.tv_sec;

		heap_timer_t *item;
		while ((item = min_heap_top(&base_heap)) != NULL) {
			if (item && item->expire <= last && item->func) { //如果时间到
				item->func(owner, data); //调用函数
				min_heap_pop(&base_heap);
				free(item);
				item = NULL;
				continue;
			} 
			break;
		}
	}
}

inline void timer_init()
{
	min_heap_ctor(&base_heap);
}

inline void timer_fini()
{
	min_heap_dtor(&base_heap);
}

inline int add_timer_event(int timerid_, time_t expire_, timer_func_t func_, void *owner_, void *data_)
{
	heap_timer_t* item = (heap_timer_t *)malloc(sizeof(heap_timer_t));
	item->expire = expire_;
	item->owner = owner_;
	item->data = data_;
	item->func = func_;
	item->timer_id = timerid_;

	return min_heap_push(&base_heap, item);
}

#define ADD_TIMER_EVENT(timerid_, expire_, func_, owner_, data_) \
	if (add_timer_event(timerid_, expire_, func_, owner_, data_)) { \
		ERROR(0, "add timer event error [id=%u]", timerid_);\
		return -1;\
	}

#endif
