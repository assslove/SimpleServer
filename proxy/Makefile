# 用于创建proxy
TARGET=proxy.so
INC = 
LIBS = `pkg-config --cflags --libs glib-2.0` `xml2-config --cflags --libs` -L/usr/local/lib -lnanc -lnanc++ 
CXXFLAGS = -g -Wall -fPIC -shared -D_GNU_SOURCE -DSERV_ONE
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
CC = g++

all : $(TARGET)

$(TARGET) : $(OBJS)
	@echo "srcs:" $(SRCS)
	@echo "objs:" $(OBJS)
	@echo $(LIBS)
	$(CC) $(CXXFLAGS) $(INC) $(LIBS) -o $(TARGET) $(OBJS)

$(OBJS) : %.o : %.cpp
	$(CC) $(CXXFLAGS) $(INC) $(LIBS) -c $< -o $@ 

clean:
	rm -f $(OBJS)  proxy.so
