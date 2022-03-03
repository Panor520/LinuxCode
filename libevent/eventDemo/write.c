#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>

#define FIFO_FILENAME "rw.fifo" 


void e_write(evutil_socket_t fd, short what,void *arg)
{	
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	
	strcpy(buf,"hello world");
	write(fd, buf, strlen(buf));
	
	printf("fifo write:%s\n",buf);
	
	sleep(1);
	return;
}

int main(int argc, char *argv[])
{	
	int fd;
	
	//写端不创建fifo文件
	//unlink(FIFO_FILENAME);
	//mkfifo(FIFO_FILENAME, 0644);

	fd = open(FIFO_FILENAME, O_WRONLY|O_NONBLOCK);
	
	struct event_base * base = event_base_new();	//创建 基础， 打地基

	struct event * event = event_new(base, fd, EV_WRITE|EV_PERSIST, e_write, NULL);	//创建事件 注意参数 EV_PERSIST的用法
	
	event_add(event, NULL);						//添加事件到地基上
	

	event_base_dispatch(base);					//循环地基上的事件，触发后自动回调时间上的函数

	event_free(event);								//释放创建的事件
	event_base_free(base);						//释放创建的地基
	close(fd);
	
	return 0;
}
