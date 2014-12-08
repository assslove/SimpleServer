
WORKDIR = /home/houbin/work/SimpleServer/

all : simple_svr simple_cli test_hashtb

simple_svr: simple_svr.c net_util.c
	gcc -g -o simple_svr simple_svr.c net_util.c log.c

simple_cli: simple_cli.c net_util.c
	gcc -g -o simple_cli simple_cli.c net_util.c log.c

test_hashtb: test_hashmap.c
	gcc test_hashmap.c -o test_hashmap `pkg-config --cflags --libs glib-2.0` 
