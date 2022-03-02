本文件夹为利用 libevent 框架 实现 fifo 管道通信的demo

build command:
	gcc read.c -o read -levent
	gcc write.c -o write -levent

test command:
	必须先启动 读端。因为读端创建了管道文件。写端直接使用
	./read       
	./write
