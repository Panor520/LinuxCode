本文件夹为I/O多路转接 poll的例子。

poll 其实就是 增加数组记录需要遍历的文件描述符的 select。逻辑大同 select_better.c.

server.c :为利用poll编写的服务器例子

build command:
		gcc ../lib/customsocket.c server.c -o server

test command:
	启动多个客户端和一个服务端进行测试。
	./server 127.0.0.1 10001
	nc 127.0.0.1 10001 
	nc 127.0.0.1 10001
	nc 127.0.0.1 10001

listen command:
	netstat -apn |grep 10001
