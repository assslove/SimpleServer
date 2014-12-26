#ifndef _NET_UTIL_H
#define _NET_UTIL_H

#include <sys/socket.h> /* basic socket definition */
#include <netinet/in.h> /* sockaddr_in and other internet def */
#include <sys/types.h>
#include <sys/epoll.h>
#include <stdint.h>

#include "mem_queue.h"
#include "list.h"

/* @brief 服务器类型
 */
enum SERV_TYPE {
	SERV_MASTER = 0, 
	SERV_WORK = 0
};

/* @brief 缓存队列的类型
 */
enum CACHE_TYPE {
	CACHE_READ = 1, 	
	CACHE_CLOSE = 2
};

/* fd 类型
 */
enum fd_type {
	fd_type_listen,
	fd_type_connect,
	fd_type_mcast, 
	fd_type_pipe, 
	fd_type_cli,  //客户端fd
	fd_type_svr,  //服务端fd
	fd_type_null, //空类型
};

/* @brief 工作进程配置项
 */
typedef struct work {
	uint32_t id;
	char ip[32];
	uint16_t port;
	uint8_t proto_type; 
	mem_queue_t rq;	 //接收队列
	mem_queue_t sq;  //发送队列
}__attribute__((packed)) work_t;

/* @brief work配置项
 */
typedef struct work_mgr {
	int nr_work;
	int nr_used;
	work_t *works; //配置项
} __attribute__((packed)) work_mgr_t;

/* @brief 连接缓存区 用于读写
 */
typedef struct fd_buff {
	int slen;		//发送缓冲长度
	int rlen;		//接收缓冲区长度
	char *sbf;			//发送缓冲区
	char *rbf;			//接收缓冲区
	int msglen;			//消息长度
	int sbf_size;		//大小
}__attribute__((packed)) fd_buff_t;

/* @brief 地址结构
 */
typedef struct fd_addr {
	uint32_t ip;
	uint16_t port;
} fd_addr_t;

/* @brief fd信息
 */
typedef struct fd_wrap {
	uint8_t type;
	int fd;
	int idx; //epinfo->fds index
	int id;  //work id index
	uint8_t flag; //标志 CACHE_TYPE
	fd_buff_t buff; //缓存
	fd_addr_t addr; //地址信息
	list_head_t node;   //在可读或者关闭链表的位置
} __attribute__((packed)) fd_wrap_t;

/* @brief 对epoll的封装
 */
typedef struct epoll_info {
	int epfd;
	struct epoll_event *evs;
	fd_wrap_t *fds;
	int maxfd;
	int maxev;
	uint32_t seq; 
	uint32_t count;
	list_head_t readlist; //待读取链表
	list_head_t closelist; //待关闭链表
}__attribute__((packed)) epoll_info_t;

typedef struct svr_setting {
	int nr_max_event;	// 最大的事件类型 epoll_create ms不需要这个参数了
	int nr_max_fd;		// fd max num
	int mem_queue_len;	// 共享内存队列长度
	int max_msg_len;	// 最大消息长度
	int max_buf_len;	//发送(接收)缓冲区最大长度 超过报错
	char svr_name[32];	//服务器名字
	int mcast_msg_len;  //组播包长
	int raw_buf_len;	//socket buf 长度
} svr_setting_t;

/*	
 *	@brief net util
 *	@ref 
 */

/* @brief 设置sock send time out
 */
int set_sock_snd_timmo(int sockfd, int millisec);

/* @brief 设置sock recv time out
 */
int set_sock_rcv_timeo(int sockfd, int millisec);

/* @brief 接收total字节数据到buf
 */
int safe_tcp_recv_n(int sockfd, void* buf, int total);

/* @brief 发送total字节数据到sockfd
 */
int safe_tcp_send_n(int sockfd, const void* buf, int total);

/* @brief 监听socket
 */
int safe_socket_listen(const char* ipaddr, in_port_t port, int type, int backlog, int bufsize);

/* @brief接收socket
 */
int safe_tcp_accept(int sockfd, struct sockaddr_in* peer, int nonblock);

/* @brief 连接指定的sock
 */
int safe_tcp_connect(const char* ipaddr, in_port_t port, int timeout, int nonblock);

/* @brief 加入epoll
 */
int add_fd_to_epinfo(int epfd, int fd, int events);

/* @brief 修改epoll的属性
 */
int mod_fd_to_epinfo(int epfd, int fd, int events);

/* @brief 
 */
int add_pfd_to_epinfo(int epfd, void *pfd, int events);

/* @brief 
 */
int mod_pfd_to_epinfo(int epfd, void *pfd, int events);

/* @brief  将消息转化为blk
 */
void raw2blk(int fd, mem_block_t *blk);

/* @brief work进程向客户端发送信息调用的接口
 */
int send_to_cli(int fd, void *msg, int len);

/* @brief work进程向所连服务器发送调用
 */
int send_to_svr(int fd, void *msg, int len);

/* @brief 释放缓存区
 */
void free_buff(fd_buff_t *buff);

#endif
