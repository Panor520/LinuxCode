本文件夹为I/O多路转接select的例子

server.c	：为普通的select实现方式，需要遍历所有的文件描述符，效率较低

server_etter.c	：添加client数组，用来记录所有需要遍历的文件描述符，增加了效率。

build command:
	gcc ../lib/customsocket.c server.c -o server -lpthread
	gcc ../lib/customsocket.c server_better.c  -o server_better -lpthread

test command:
	server: ./server 127.0.0.1 10001
	client:	nc 127.0.0.1 10001

		
	server_better: ./server 127.0.0.1 10001
	client:	nc 127.0.0.1 10001
	
