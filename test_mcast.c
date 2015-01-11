/*
 * =====================================================================================
 *
 *       Filename:  test_mcast.c
 *
 *    Description:  test mcast 服务端
 *
 *        Version:  1.0
 *        Created:  2015年01月04日 10时18分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int set_mcast_ttl(int fd, int ttl)
{
	return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
}

int set_mcast_loop(int fd, int loop)
{
	return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
}

int set_mcast_if(int fd, struct in_addr in)
{
	return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &in, sizeof(struct in_addr));
}

int join_mcast(int fd, struct ip_mreq *req)
{
	return setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, req, sizeof(struct ip_mreq));
}

int leave_mcast(int fd, struct ip_mreq *req)
{
	return setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, req, sizeof(struct ip_mreq));
}

int main(int argc, char* argv[])
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		ERROR(0, "socket error");
		return 1;
	}

	set_mcast_ttl(sockfd, 1);
	set_mcast_loop(sockfd, 1);


	return 0;
}
