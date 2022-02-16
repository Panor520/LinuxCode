本文件夹是利用自定义函数实现通讯的实例

build：
	gcc server.c customsocket.c -o server
	gcc client.c customsocket.c -o client

test code:
	./client 10.219.10.193 10001
	./server 10.219.10.193 10001
