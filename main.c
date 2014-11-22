/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  test 
 *
 *        Version:  1.0
 *        Created:  04/06/2014 11:38:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> /* basic socket definition */
#include <netinet/in.h> /* sockaddr_in and other internet def */
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h> /* bzero */

enum RTYPE {
	SUCCESS = 0,
	ERROR = -1 
};

int main(int argc, char* argv[]) 
{	
	int listenfd, connfd;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		printf("socket error\n");
		return ERROR;
	}

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8000);

	int ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0) {
		printf("socket bind error\n");
		return ERROR;
	}

	ret = listen(listenfd, 5);
	if (ret < 0) {
		printf("socket listen error\n");
		return ERROR;
	}

	int stop = 0;
	char buf[1024];
	while (!stop) {
		int clilen = sizeof(struct sockaddr_in);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

		int n = 0;	

read_again:
		while ((n = read(connfd, buf, 1024) > 0)) { //阻塞的
			ret = write(connfd, buf, n);	
			if (ret < 0) {
				printf("write error");
				return ERROR;
			}
		}

		if (n < 0) {
			if (errno == EINTR) {
				goto read_again;
			} else {
				printf("socket read error\n");
				return ERROR;
			}
		}
	}

	return 0;
}

