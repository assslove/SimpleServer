echo_server: main.c
	gcc -g -o echo_server main.c
clean:
	rm -rf *.o
