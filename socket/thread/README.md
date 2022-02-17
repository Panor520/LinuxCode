本文件夹：
		是C语言实现的多进程网络服务器。

		利用自定义函数库编译程序。

build command:
	gcc ../lib/customsocket.c server.c -o server
	gcc ../lib/customsocket.c client.c -o client

test command: 一个server 多个client

	./server 127.0.0.1 10001
	./client 127.0.0.1 10001
	./client 127.0.0.1 10001
	./client 127.0.0.1 10001
