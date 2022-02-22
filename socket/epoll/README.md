本文件夹是epoll的实例

epoll的本质是红黑树，每个结点涵盖一个文件描述符及相应要获取的事件，可有效获取到每个发生事件的结点。

build command：
   gcc ../lib/customsocket.c server.c -o server -lpthread


test command:
	./server 127.0.0.1 10001
	nc 127.0.0.1 10001
	nc 127.0.0.1 10001
