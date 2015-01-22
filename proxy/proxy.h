#ifndef _PROXY_H_
#define _PROXY_H_


#include <libnanc++/singleton.h>


/* @brief 代理服务器实现逻辑
 * @note 本服务器只适用于单角色多服或者单服
 */
class Proxy {

};

int handle_proxy(int fd, void *msg, int len);

#endif 
