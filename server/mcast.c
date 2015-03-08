/*
 * =====================================================================================
 *
 *       Filename:  mcast.c
 *
 *    Description:  组播使用 (1-服务发现; 2-重载业务逻辑 3-重载配置)
 *
 *        Version:  1.0
 *        Created:  2015年03月05日 15时33分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <libnanc/log.h>

#include "mcast.h"

int mcast_cli_init(char *mcast_ip, uint16_t mcast_port, char *local_ip) 
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		ERROR(0, "failed create mcast fd [%s]", strerror(errno));
		return -1;
	}

	struct sockaddr_in local_sa;
	local_sa.sin_family = AF_INET;
	local_sa.sin_port = htons(mcast_port);
	local_sa.sin_addr.s_addr = INADDR_ANY;

	int flag = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
		ERROR(0, "set sock reuseaddr failed [%s]", strerror(errno));
		return -1;
	}

	int ret = bind(sockfd, (struct sockaddr*)&local_sa, sizeof(struct sockaddr_in));
	if (ret == -1) {
		ERROR(0, "mcast bind failed [%s]", strerror(errno));
		return -1;
	}

	if (set_mcast_ttl(sockfd, 1) == -1) {
		ERROR(0, "set mcast ttl failed [%s]", strerror(errno));
		return -1;
	}

	if (set_mcast_loop(sockfd, 1) == -1) {
		ERROR(0, "set mcast loop failed [%s]", strerror(errno));
		return -1;
	}

	return 0;	

	struct ip_mreq req;
	req.imr_multiaddr.s_addr = inet_addr(mcast_ip);
	req.imr_interface.s_addr = inet_addr(local_ip);

	if (join_mcast(sockfd, &req) == -1) {
		ERROR(0, "join mcast failed [%s]", strerror(errno));
		return -1;
	}

	return sockfd;
}
