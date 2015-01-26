# 用于创建proxy
TARGET=proxy.so 
DATA_TARGET=data.so
INC = -I/usr/local/include/libnanc -I/usr/local/include/libnanc++
LIBS = `pkg-config --cflags --libs glib-2.0 libxml-2.0` -L/usr/local/lib -lxml2 -lnanc -lnanc++
CXXFLAGS = -g -Wall -fPIC -shared -D_GNU_SOURCE -DSERV_ONE 
#SRCS = $(wildcard *.cpp)
#OBJS = $(patsubst %.cpp, %.o, $(SRCS))
OBJS = proxy_serv.o proxy.o router.o
DATA_OBJS = data.o
CC = g++

all : $(TARGET) $(DATA_TARGET)

$(TARGET) : $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS) $(CXXFLAGS) $(INC) $(LIBS)

$(DATA_TARGET) : data.cpp
	$(CC) -o $(DATA_TARGET) $(DATA_OBJS) $(CXXFLAGS)

$(OBJS) : %.o : %.cpp
	$(CC) -c -o $@ $< $(CXXFLAGS) $(INC) $(LIBS) 

clean:
	rm -f $(OBJS)  proxy.so
