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

#endif 
