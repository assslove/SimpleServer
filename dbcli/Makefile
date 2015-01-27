
CC = g++
CXXFLAGS = -g -Wall

all : mysqlcli
	$(CC)  mysqlcli.o -o mysqlcli
mysqlcli.o : mysqlcli.cpp
	$(CC) -c -o mysqlcli.o $(CXXFLAGS) mysqlcli.cpp

clean:
	rm -rf *.o mysqlcli
