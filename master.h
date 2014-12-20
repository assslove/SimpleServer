#ifndef MASTER_H_
#define MASTER_H_

typedef struct svr_setting {
	int nr_max_event;	// 最大的事件类型 epoll_create ms不需要这个参数了
	int nr_max_fd;		// fd max num
	int mem_queue_len;	// 共享内存队列长度
	int max_msg_len;	// 最大消息长度
	int max_buf_len;	//发送(接收)缓冲区最大长度 超过报错
	char svr_name[32];	//服务器名字
} svr_setting_t;

/* @brief master init
 * @note 
 */
int master_init();

/* @brief 
 */
int master_listen(int i);

/* @brief 
 */
int master_dispatch();

/* @brief 
 */
int master_fini();

/* @brief 增加fd到epinfo
 */
int add_fdinfo_to_epinfo(int fd, int idx, int type, int ip, uint16_t port);

/*  @brief 处理cli
 */
int handle_cli(int fd);

/* @brief  handle read
 */
int handle_read(int fd);

/* @brief 处理管道的读取
 * @note 直接就可以
 */
int handle_pipe(int fd);

/* @brief 处理往客户端发送消息
 */
int handle_mq_send();

/* @brief 发送消息块
 */
int do_blk_send(mem_block_t &blk);

/* @brief 处理消息的发送
 */
int do_fd_send(int fd, void *data, int len);

/* @brief 处理消息的写入
 */
int do_fd_write(int fd);
#endif
