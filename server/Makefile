# 简单服务器 一父多子 

INSTALL_DIR=/usr/local/include/sserv
LIBS = `pkg-config --cflags --libs glib-2.0`

all : simple_svr epoll_cli test_serv.so data.so

simple_svr: simple_svr.c net_util.c mem_queue.c conf.h log.c util.c master.c work.c global.c  fds.c outer.c mcast.c
	gcc -g -Wall -DENABLE_TRACE \
		 -o simple_svr  simple_svr.c global.c net_util.c log.c util.c mem_queue.c conf.c master.c work.c  fds.c outer.c mcast.c\
		 `pkg-config --cflags --libs glib-2.0`  -ldl -rdynamic
epoll_cli: epoll_cli.c
	gcc -g -o epoll_cli epoll_cli.c
test_serv.so: test_serv.cpp switch.cpp
	g++ -g -shared -fPIC test_serv.cpp switch.cpp -o test_serv.so `pkg-config --cflags --libs glib-2.0` -I./ -D_GNU_SOURCE  -I./../../libnanc/src/
data.so: data.c
	gcc -g -shared -fPIC data.c -o data.so
clean:
	rm -rf *.o simple_svr test_serv.so epoll_cli

install: 
	sudo rm -rf $(INSTALL_DIR)
	sudo mkdir $(INSTALL_DIR)
	sudo cp -r *.h $(INSTALL_DIR) 
