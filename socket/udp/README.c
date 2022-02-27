本文件夹为 udp 通信demo
支持并发。（原因自行百度）

build command:
	gcc server.c -o server
	gcc client.c -o client


test command:
	./server 127.0.0.1 10001
	./client 127.0.0.1 10001
	./client 127.0.0.1 10001
	./client 127.0.0.1 10001

