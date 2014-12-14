#ifndef FDS_H_
#define FDS_H_


#include <sys/socket.h>
#include <stdint.h>

/* @brief 客户端fd信息
 */
typedef struct fdsess {
	int fd;
	int id; //work
	int type;
	int port;
	int ip;
} fdsess_t;

/* @brief 保存fd
 */
int save_fd(int fd, int id, int type, uint32_t ip, uint16_t port);

/* @brief 初始化fds
 */
int	init_fds();


/* @brief 获取fd
 */
fdsess_t *get_fd(int fd);

#endif 
