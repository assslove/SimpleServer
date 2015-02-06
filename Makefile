WORKDIR = /home/houbin/work/SimpleServer/
LIBS = `pkg-config --cflags --libs glib-2.0`

all : test_hashtb test_mq test_mq2 test_log test_load_conf epoll_cli

test_hashtb: test_hashmap.c
	gcc test_hashmap.c -o test_hashmap `pkg-config --cflags --libs glib-2.0` 

test_mq: 	test_mem_queue.c mem_queue.c util.c
	gcc -g test_mem_queue.c mem_queue.c util.c -o test_mq 

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
test_serv.so: test_serv.cpp switch.cpp
	g++ -g -shared -fPIC test_serv.cpp switch.cpp -o test_serv.so `pkg-config --cflags --libs glib-2.0` -I./ -D_GNU_SOURCE 
data.so: data.c
	gcc -g -shared -fPIC data.c -o data.so
test_mcast : test_mcast.c
	gcc -g test_mcast.c -o test_mcast
mcast_cli: mcast_cli.c
	gcc -g mcast_cli.c -o mcast_cli
test_pthread: test_pthread.c
	gcc -g -Wall test_pthread.c -o test_pthread -lpthread
test_sem: test_sem.c
	gcc -g -Wall test_sem.c -o test_sem -lpthread
clean:
	rm -rf *.o simple_svr simple_cli test_hashtb test_mq test_mq2 test_log test_load_conf test_serv.so

