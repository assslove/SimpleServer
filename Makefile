
WORKDIR = /home/houbin/work/SimpleServer/

all : simple_svr simple_cli test_hashtb

simple_svr: simple_svr.c net_util.c
	gcc -g -o simple_svr simple_svr.c net_util.c log.c

simple_cli: simple_cli.c net_util.c
	gcc -g -o simple_cli simple_cli.c net_util.c log.c

test_hashtb: test_hashmap.c
	gcc -g -o test_hashtb -lglib-2.0 -L/home/houbin/local/glib-2.0/lib -I/home/houbin/local/glib-2.0/include/glib-2.0 \
	 	-I/home/houbin/local/glib-2.0/lib/glib-2.0/include test_hashmap.c
