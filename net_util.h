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

/* fd 类型
 */
enum fd_type {
	fd_type_listen,
	fd_type_connect,
	fd_type_mcast, 
	fd_type_pipe, 
	fd_type_cli,  //客户端fd
	fd_type_svr,  //服务端fd
};

/* @brief 工作进程配置项
 */
typedef struct work {
	uint32_t id;
	char ip[32];
	uint16_t port;
	uint8_t proto_type; 
	mem_queue_t rq;	 //接收队列
	mem_queue_t wq;  //发送队列
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
typedef struct fd_buff{
	int slen;		//发送缓冲长度
	int rlen;		//接收缓冲区长度
	char *sbf;			//发送缓冲区
	char *rbf;			//接收缓冲区
	int msglen;			//消息长度
}__attribute__((packed)) fd_buff_t;

/* @brief 地址结构
 */
typedef struct fd_addr {
	uint32_t ip;
	uint16_t port;
} fd_addr_t;

/* @brief fd信息
 */
typedef struct {
	uint8_t type;
	int fd;
	int idx; //epinfo->fds index
	int id;  //work id index
	fd_buff_t buff;
	fd_addr_t addr;
	list_head_t closelist; //待关闭链表
	list_head_t readlist;  //待读取链表
} __attribute__((packed)) fd_wrap_t;

/* @brief 对epoll的封装
 */
typedef struct {
	uint32_t seq; 
	int epfd;
	struct epoll_event *evs;
	fd_wrap_t *fds;
	int max_fd;
	int max_ev;
}__attribute__((packed)) epoll_info_t;


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

/* @brief 
 */
void raw2blk(int fd, mem_block_t &blk);

/* @brief work进程向客户端发送信息调用的接口
 */
int send_to_cli(int fd, void *msg, int len);

/* @brief work进程向所连服务器发送调用
 */
int send_to_svr(int fd, void *msg, int len);

#endif
