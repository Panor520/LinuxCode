#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <arpa/inet.h>

void cb_read(struct bufferevent * bev, void * ptr)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	
	bufferevent_read(bev, buf, sizeof(buf));

	printf("get:%s\n",buf);

	bufferevent_write(bev, buf, strlen(buf)+1);
}

void cb_write(struct bufferevent * bev, void * ptr)
{
	printf("client send data succsee.\n");
}

void cb_event(struct bufferevent * bev, short events, void * ptr)
{	
	if(events & BEV_EVENT_EOF)
	{
		printf("connection closed.\n");
	}
	else if(events & BEV_EVENT_ERROR)
	{
		printf("some error.\n");
	}
	else if(events & BEV_EVENT_CONNECTED)
	{
		printf("server has been connected.\n");
	}

	bufferevent_free(bev);
	
}

void cb_ev(evutil_socket_t fd, short what,void *arg)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	int n = read(fd, buf, sizeof(buf));						//读标准输入数据

	struct bufferevent * bev = (struct bufferevent *)arg;	
	
	bufferevent_write(bev, buf, n+1);						//给服务器发送数据

	return;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr.s_addr );

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	struct event_base * base = event_base_new();										//创建 libevent 底座
	
	struct bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);	//创建事件
	
	bufferevent_enable(bev, EV_READ);													//启用读端	
	bufferevent_setcb(bev, cb_read, cb_write, cb_event,base);							//设置事件回调函数

	bufferevent_socket_connect(bev, (struct sockaddr*)&addr_s,sizeof(addr_s));							//建立和服务端的连接
	
	
	struct event * ev = event_new(base, STDIN_FILENO, EV_READ|EV_PERSIST, cb_ev, bev);//创建 读标准输入事件
	
	event_add(ev, NULL);																//将标准输入事件 加入底座

	event_base_dispatch(base);															//设置底座循环

	event_free(ev);																		//释放读标准输入事件
	event_base_free(base);																//释放 底座
	return 0;
}
