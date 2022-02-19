本文件夹为I/O多路转接的例子

build command:
	gcc ../lib/customsocket.c server.c -o server -lpthread

test command:
	server: ./server 127.0.0.1 10001
	client:	nc 127.0.0.1 10001
