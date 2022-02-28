本文件夹为进程间利用 本地套接字 通信的例子

AF_UNIX/AF_LOCAL

注意与网络通讯的套接字的差异。


bulid command:
	gcc server.c -o server
	gcc client.c -o client


test command:
	./server
	./client
