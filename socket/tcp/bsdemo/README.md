本文件夹为linux网络编程中 browser/server 模型demo


server.c :
	由epoll写的服务端程序，包含注释，可仔细查看。


build command：
	gcc server.c -o server

test command:
	server端：	
		./server ip port 文件夹路径
		例：./server 192.168.3.250 10001 /home/pl/Desktop/LinuxCode/socket/tcp/bsdemo/filedir/ 
	brwoser端：
		http://192.168.3.250:10001

debug command:
	gcc server.c -o server -g
	gdb server
	set args 192.168.3.250 10001 /home/pl/Desktop/LinuxCode/socket/tcp/bsdemo/filedir/
	r

