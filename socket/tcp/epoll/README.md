本文件夹是epoll的实例

一个为普通的epoll实例，一个为增强版的（这个貌似有点问题，有bug未修复）。

epoll的本质是红黑树，每个结点涵盖一个文件描述符及相应要获取的事件，可有效获取到每个发生事件的结点。

普通epoll实例：

	build command：
		gcc ../lib/customsocket.c server.c -o server -lpthread


	test command:
		./server 127.0.0.1 10001
		nc 127.0.0.1 10001
		nc 127.0.0.1 10001

增强版：
	build command:
		gcc ../libcustomsocket.c server_better.c -o server_better -lpthread
	
	test command:
		./server_better 127.0.0.1 10002
		nc 127.0.0.1 10002
		nc 127.0.0.1 10002
