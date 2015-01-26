# 用于创建proxy
TARGET=proxy.so
INC = -I/usr/local/include/libnanc -I/usr/local/include/libnanc++
LIBS = `pkg-config --cflags --libs glib-2.0 libxml-2.0` -L/usr/local/lib -lxml2 -lnanc -lnanc++
CXXFLAGS = -g -Wall -fPIC -shared -D_GNU_SOURCE -DSERV_ONE 
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
CC = g++

all : $(TARGET)

$(TARGET) : $(OBJS)
	@echo "srcs:" $(SRCS)
	@echo "objs:" $(OBJS)
	@echo $(LIBS)
	$(CC)  -o $(TARGET) $(OBJS) $(CXXFLAGS) $(INC) $(LIBS)

$(OBJS) : %.o : %.cpp
	$(CC) -c -o $@ $< $(CXXFLAGS) $(INC) $(LIBS) 

clean:
	rm -f $(OBJS)  proxy.so
