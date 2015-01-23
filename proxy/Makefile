# 用于创建proxy
TARGET=proxy.so
CXXFLAGS = -g -Wall -fPIC -shared -D_GNU_SOURCE -DSERV_ONE
LIBS = -lnanc -lnanc++ `pkg-config --cflags --libs glib-2.0`
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
CC = g++

$(TARGET) : $(OBJS)
	@echo "srcs:" $(SRCS)
	@echo "objs:" $(OBJS)
	@echo "complie to objs"
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) $(INC) -o $(TARGET)

clean:
	rm -f $(OBJECTS)  proxy.so
