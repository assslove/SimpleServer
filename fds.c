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

#include <glib.h>
#include "global.h"

static void do_free_fd(void* fd)
{
	g_free(fd);
	fd = NULL;
}

int	init_fds()
{
	fds = g_hash_table_new_full(g_int_hash, g_int_equal, 0, do_free_fd);
	return 0;
}

fdsess_t *get_fd(int fd)
{
	return  g_hash_table_lookup(fds, &fd);;
}

int save_fd(int fd, int id, int type, sockaddr_in *addr)
{
	fdsess_t *sess;
	g_hash_table_insert(fds, &(fdsess->fd), fdsess);
}
