#ifndef MCAST_H_
#define MCAST_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* @brief 组播类型定义
 */
enum MCAST_CMD {
	MCAST_SERV_NOTI = 1, //服务发现
	MCAST_RELOAD_SO = 2, //重载业务逻辑
	MCAST_RELOAD_CONF = 3 //重载配置文件
};

/* @brief 组播包格式定义
 */
typedef struct mcast_pkg {
	int len;			//协议长度
	uint8_t mcast_type; //组播类型,见MCAST_TYPE
	uint8_t data[];		//协议数据
} __attribute__((packed)) mcast_pkg_t;

/* @brief 服务通知信息
 */
typedef struct serv_noti {
	uint32_t ip;	//服务ip
	uint16_t port;	//服务端口号
	uint16_t id;	//服务id
	char servname[32]; //服务名称
} __attribute__((packed)) serv_noti_t;

/* @brief 重载业务逻辑信息
 */
typedef struct reload_so {
	uint16_t id;	 //服务器id 0表示所有进程
	char servname[32]; //服务名字
} __attribute__((packed)) reload_so_t;

/* @brief 重载业务逻辑名字
 */
typedef struct reload_conf {
	uint16_t id;		//服务器id 0表示所有进程
	uint16_t conf_id;	//conf文件id;
} __attribute__((packed)) reload_conf_t;

/* @brief 设置组播ttl
 */
inline int set_mcast_ttl(int fd, int ttl)
{
	return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
}

/* @brief 设置组播loop
 */
inline int set_mcast_loop(int fd, int loop)
{
	return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
}

/* @brief 设置组播接口
 */
inline int set_mcast_if(int fd, struct in_addr in)
{
	return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &in, sizeof(struct in_addr));
}

/* @brief 加入组播
 */
inline int join_mcast(int fd, struct ip_mreq *req)
{
	return setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, req, sizeof(struct ip_mreq));
}

/* @brief 离开组播
 */
inline int leave_mcast(int fd, struct ip_mreq *req)
{
	return setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, req, sizeof(struct ip_mreq));
}


/* @brief 初始化组播接口
 * @param mcast_ip 组播ip
 * @param mcast_port 组播端口号
 * @param local_ip 本地ip
 *
 * @return >0 组播fd -1 error
 */
int mcast_cli_init(char *mcast_ip, uint16_t mcast_port, char *local_ip); 

#endif
