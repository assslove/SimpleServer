/*
 * =====================================================================================
 *
 *       Filename:  simple_cli.c
 *
 *    Description:  客户端测试
 *
 *        Version:  1.0
 *        Created:  12/06/2014 08:37:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
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
#include <strings.h>   
#include <string.h>   
#include <sys/epoll.h>

#include "util.h"
#include "log.h"

int main(int argc, char* argv[])
{
	int cli_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &(servaddr.sin_addr));
	servaddr.sin_port = htons(8000);

	int err = 0;
	err = connect(cli_sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));

	if (err == -1) {
		ERROR(0, "%s", strerror(errno));
		return 0;
	}
	
	int stop = 0;
	int i = 0;
	set_io_nonblock(cli_sock, 1);
	while (!stop) {
		sleep(2);
		i++;	
		char buf[32];	
		sprintf(buf, "hello, world[id = %d]", i);
		buf[32] = '\0';
		send(cli_sock, buf, 32, 0);
		DEBUG(cli_sock, "send to svr[%s]", buf);

		char recv_buf[32] = {'\0'};
		int ret = recv(cli_sock, recv_buf, 1024, 0);
		if (ret == 0) {
			DEBUG(0, "serv closed");
			break;
		} else if (ret == -1) {
			DEBUG(0, "no data");
		} else {
			DEBUG(cli_sock, "svr to cli[%s]", recv_buf);
		}
	}

	ERROR(0, "%s", strerror(errno));
	close(cli_sock);

	return 0;
}
