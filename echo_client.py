#!/usr/bin/env python

from socket import *

HOST = "127.0.0.1"
PORT = 8000
BUFSIZE = 1024
ADDR = (HOST, PORT)

cli_sock = socket(AF_INET, SOCK_STREAM)
cli_sock.connect(ADDR)

while True:
    data = raw_input("please input some message > ")
    if not data:
        break

    print data
    cli_sock.send("%s \r\n" % data)
    data = ""
    data = cli_sock.recv(BUFSIZE)

    if not data:
        break

    print data
    data = ""

cli_sock.close()
