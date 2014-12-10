#ifndef _NET_UTIL_H
#define _NET_UTIL_H

#include <sys/socket.h> /* basic socket definition */
#include <netinet/in.h> /* sockaddr_in and other internet def */
#include <sys/types.h>

/*	
 *	@brief net util
 *	@ref 
 */

/* @brief 设置io非阻塞
 */
int set_io_nonblock(int fd, int nonblock);

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

#endif
