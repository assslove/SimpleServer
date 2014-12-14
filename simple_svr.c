/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  epoll 	svr 
 *					muti process
 *					provide some interface for man
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
#include <strings.h>   
#include <string.h>   
#include <sys/epoll.h>
#include <sys/mman.h>
#include <arpa/inet.h>

#include "net_util.h"
#include "mem_queue.h"
#include "log.h"
#include "conf.h"
#include "global.h"
#include "util.h"
#include "master.h"
#include "work.h"

int main(int argc, char* argv[]) 
{	
	//load conf
	load_conf();	
	//chg limit
	init_rlimit();
	//save args
	save_args(argc, argv);
	//chg serv name
	chg_proc_title("SimpleServer");
	//daemon mode
	daemon(0, 0);

	//handle signal
	//handle pipe
	
	//master_init
	master_init();

	int i = 0;
	for (; i < workmgr.nr_used; i++) {
		int pid = fork();
		if (pid <= 0) {
			ERROR(0, "create work fail[%d][%s]", i, strerror(errno));
			goto fail;	
		} else if (pid == 0) { //child
			work_init(i);
			work_dispatch(i);
			work_fini(i);
			exit(0);
		} else { //parent
			chl_pids[i] = pid;
			int ret = master_listen(i);
			if (ret == -1) {
				ERROR(0, "%s", strerror(errno));
				goto fail;
			}
		}
	}

	//master loop
	master_dispatch();
fail:
	//master fini();
	master_fini();

	return 0;
}

