all : mysqlcli
	gcc mysqlcli.o -o mysqlcli
mysqlcli.o : mysqlcli.c
	gcc mysqlcli.c -c mysqlcli.o

clean:
	rm -rf *.o mysqlcli
