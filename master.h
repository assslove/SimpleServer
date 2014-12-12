#ifndef MASTER_H_
#define MASTER_H_

typedef struct svr_setting {
	int nr_max_event;	// 最大的事件类型 epoll_create ms不需要这个参数了
	int mem_queue_len;	// 共享内存队列长度
	int max_msg_len;	// 最大消息长度
	int max_buf_len;	//发送(接收)缓冲区最大长度 超过报错
	char serv_name[32];	//服务器名字
} svr_setting_t;

#endif
