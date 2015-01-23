# 用于创建proxy
TARGET=proxy.so
CXXFLAGS = -g -Wall -fPIC -shared -D_GNU_SOURCE -DSERV_ONE
INC = `pkg-config --cflags glib-2.0` `xml2-config --cflags` 
LIBS = -lnanc -lnanc++ `pkg-config --libs glib-2.0` `xml2-config --libs` 
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
CC = g++

$(TARGET) : $(OBJS)
	@echo "srcs:" $(SRCS)
	@echo "objs:" $(OBJS)
	@echo "complie to objs"
	@echo $(LIBS)
	$(CC) $(OBJS) $(CXXFLAGS) $(INC) $(LIBS) -o $(TARGET)
%.o : %.cpp
	$(CC) $(OBJS) $(CXXFLAGS) $(INC) $(LIBS) -c $< -o $@ 

clean:
	rm -f $(OBJECTS)  proxy.so
