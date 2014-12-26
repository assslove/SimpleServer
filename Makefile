
WORKDIR = /home/houbin/work/SimpleServer/
LIBS = `pkg-config --cflags --libs glib-2.0`

all : simple_svr simple_cli test_hashtb test_mq test_mq2 test_log test_load_conf epoll_cli

simple_svr: simple_svr.c net_util.c mem_queue.c conf.h log.c util.c master.c work.c global.c  fds.c outer.c
	gcc -g -Wall -DENABLE_TRACE \
		 -o simple_svr  simple_svr.c global.c net_util.c log.c util.c mem_queue.c conf.c master.c work.c  fds.c outer.c\
		 `pkg-config --cflags --libs glib-2.0`  -ldl

simple_cli: simple_cli.c util.c
	gcc -g -o simple_cli simple_cli.c util.c log.c

test_hashtb: test_hashmap.c
	gcc test_hashmap.c -o test_hashmap `pkg-config --cflags --libs glib-2.0` 

test_mq: 	test_mem_queue.c mem_queue.c
	gcc -g test_mem_queue.c mem_queue.c -o test_mq

test_mq2: 	test_mem_queue_fork.c mem_queue.c
	gcc -g test_mem_queue_fork.c mem_queue.c -o test_mq2

test_log: test_log.c log.c
	gcc -g -o test_log test_log.c log.c
test_load_conf: test_load_conf.c
	gcc -g -o test_load_conf test_load_conf.c `pkg-config --cflags --libs glib-2.0` 
test_linklist: test_linklist.c list.h
	gcc -g -o test_linklist test_linklist.c
epoll_cli: epoll_cli.c
	gcc -g -o epoll_cli epoll_cli.c
test_serv.so: test_serv.cpp log.c
	g++ -g -shared -fPIC test_serv.cpp log.c -o test_serv.so 

clean:
	rm -rf *.o simple_svr simple_cli test_hashtb test_mq test_mq2 test_log test_load_conf test_serv.so
