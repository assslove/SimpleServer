/*
 * =====================================================================================
 *
 *       Filename:  mcast_cli.c
 *
 *    Description:  mcast 客户端 用于接收消息
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
#include <stdlib.h>
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
		printf("socket error");
		return 1;
	}

	struct sockaddr_in local_sa;
	local_sa.sin_family = AF_INET;
	local_sa.sin_port = htons(8888);
	local_sa.sin_addr.s_addr = INADDR_ANY;

	int flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	int ret = bind(sockfd, (struct sockaddr*)&local_sa, sizeof(local_sa));

	if (ret < 0) {
		printf("bind error\n");
		return 1;
	}

	if (set_mcast_ttl(sockfd, 1) == -1) {
		printf("set mcast ttl error\n");
		return 1;
	}

	if (set_mcast_loop(sockfd, 1) == -1) {
		printf("set mcast loop error\n");
		return 1;
	}

	struct ip_mreq req;
	req.imr_multiaddr.s_addr = inet_addr("239.0.0.2");
	req.imr_interface.s_addr = inet_addr("172.21.174.115");

	if (join_mcast(sockfd, &req) == -1) {
		printf("join mcast error\n");
		return 1;
	}

	char buff[1024];
	int i = 1;
	struct sockaddr_in from_sa;
	int sa_len = sizeof(struct sockaddr);

	for (;;) {
		memset(buff, 0, sizeof(buff));
		int len = recvfrom(sockfd, buff, 1024, 0, (struct sockaddr*)&from_sa, &sa_len);
		if (len < 0) {
			printf("recv error\n");
			return 1;
		}

		printf("recv mcast %s\n", buff);
	}
	
	leave_mcast(sockfd, &req);

	close(sockfd);
	return 0;
}
