LIBS = `pkg-config --cflags --libs glib-2.0`

all : ev_serv

ev_serv: ev_serv.c log.c global.c
	gcc -g -Wall -DENABLE_TRACE -o ev_serv ev_serv.c log.c global.c\
		 `pkg-config --cflags --libs glib-2.0`  -ldl -rdynamic
clean:
	rm -rf *.o ev_serv
