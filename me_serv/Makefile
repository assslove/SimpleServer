# 生成多进程处理的网络服务器，可用于执行数据库操作等功能
#

INSTALL_DIR = /usr/local/include/meserv
CFLAGS =  -g -Wall -Wunused-function -lpthread -ldl -DENABLE_TRACE -I/usr/include/libnanc -L/usr/lib/libnanc -lnanc
LIBS = `pkg-config --cflags --libs glib-2.0`
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

all : meserv

meserv : $(OBJS)
	gcc $(OBJS) -o meserv

%.o : %.c
	gcc $(CFLAGS) $(LIBS) -c  $< -o $@

clean:
	-rm -f $(OBJECTS) meserv

install:
	sudo rm -rf $(INSTALL_DIR)
	sudo mkdir $(INSTALL_DIR)
	sudo cp -r *.h $(INSTALL_DIR) 
