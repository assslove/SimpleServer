/*
 * =====================================================================================
 *
 *       Filename:  fds.c
 *
 *    Description:  对fd的管理
 *
 *        Version:  1.0
 *        Created:  12/07/2014 09:31:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	xiaohou, houbin-12@163.com
 *   Organization:  XiaoHou, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include "global.h"

int	init_fds()
{
	memset(fds, 0, sizeof(fds));
	return 0;
}

fdsess_t *get_fd(int fd)
{
	return  &fds[fd];
}

int save_fd(int fd, int id, int type, sockaddr_in *addr)
{
	fdsess *pfd = &fds[fd];
	pfd->fd = fd;
	pfd->id = id;
	pfd->type = type;
	pfd->port = addr->sin_port;
	pfd->ip = addr->sin_addr->s_addr;
	return 0;
}
