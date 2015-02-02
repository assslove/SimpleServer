# 用于创建proxy
TARGET=dbserv
INC = -I/usr/local/include/libnanc -I/usr/local/include/libnanc++ -I/usr/local/include/dbcli/ -I/usr/include/mysql -I./proto/lib
LIBS = -L/usr/local/lib -lxml2 -lnanc -lnanc++ -ldbcli -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl -lprotobuf
CXXFLAGS = -g -Wall -D_GNU_SOURCE 
#SRCS = $(wildcard *.cpp)
#OBJS = $(patsubst %.cpp, %.o, $(SRCS))
OBJS = dbserv.o dispatch.o user.o limit.o
CC = g++

all : $(TARGET) 

$(TARGET) : $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS) $(CXXFLAGS) $(INC) $(LIBS)

$(OBJS) : %.o : %.cpp
	$(CC) -c -o $@ $< $(CXXFLAGS) $(INC) $(LIBS) 

clean:
	rm -f $(OBJS)  $(TARGET)
