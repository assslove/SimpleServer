all : simple_svr simple_cli

simple_svr: simple_svr.c net_util.c
	gcc -g -o simple_svr simple_svr.c net_util.c log.c

simple_cli: simple_cli.c net_util.c
	gcc -g -o simple_cli simple_cli.c net_util.c log.c

