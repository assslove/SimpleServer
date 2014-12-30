all : text.so data.so

text.so: test_serv.cpp
	g++ -g -shared -fPIC test_serv.cpp -o text.so `pkg-config --cflags --libs glib-2.0` -I./../ -D_GNU_SOURCE 
data.so: data.c
	gcc -g -shared -fPIC data.c -o data.so

clean:
	rm  -rf data.so text.so
