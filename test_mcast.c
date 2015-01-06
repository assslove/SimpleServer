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
	local_sa.sin_port = htons(10001);
	local_sa.sin_addr.s_addr = inet_addr("172.21.174.115");

	if (bind(sockfd, (struct sockaddr *)&local_sa, sizeof(struct sockaddr)) == -1) {
		printf("err bind\n");
		return 1;
	}

	struct sockaddr_in mcast_sa;
	mcast_sa.sin_family = AF_INET;
	mcast_sa.sin_port = htons(8888);
	mcast_sa.sin_addr.s_addr = inet_addr("239.0.0.2");

	char buff[1024];
	int i = 1;
	struct sockaddr_in cli_sa;
	
	for (;;) {
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "helloworld_%d", ++i);
		int size = sendto(sockfd, buff, strlen(buff) + 1, 0, (struct sockaddr *)&mcast_sa, sizeof(struct sockaddr_in));
		if (size < 0) {
			printf("send to error\n");
			return 1;
		}

		printf("send successful\n");
		sleep(2);
	}

	close(sockfd);
	return 0;
}
