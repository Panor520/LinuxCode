#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

void cb_read(struct bufferevent *bev, void *ctx)	//读回调
{
	int n, i;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	n = bufferevent_read(bev, buf, sizeof(buf));
	for(i = 0; i < n; i++)
		buf[i] = toupper(buf[i]);
	
	bufferevent_write(bev, buf, n);
	
	sleep(1);
	return;
}

void cb_write(struct bufferevent *bev, void *ctx)	//写回调 bufferevent_write成功才出发此回调
{
	printf("server: send successful\n");
	return;
}


void cb_event(struct bufferevent *bev, short events, void *ctx)	//其他事件回调
{
	if(events & BEV_EVENT_EOF)
	{
	//	printf("client closed.\n");
	}
	else if(events & BEV_EVENT_ERROR)
	{
		printf("other error.\n");
	}
	
	//bufferevent_free(bev);
	
	//printf("bufferevent free success.\n");
	return;
}

void cb_listener(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int len, void *ptr)
{
	char ip_c[16];
	struct sockaddr_in * addr_c = (struct sockaddr_in *)addr;
	printf("connected %s %d..\n"
			,inet_ntop(AF_INET, &addr_c->sin_addr.s_addr, ip_c, sizeof(ip_c))
			,ntohs(addr_c->sin_port));														 //打印出连上的客户端连接

	struct event_base * base = (struct event_base*)ptr;									 //通过参数拿到底座

	struct bufferevent * bufevent = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);//创建缓冲区事件
	
	
	bufferevent_setcb(bufevent, cb_read, cb_write, cb_event, NULL);								 //设置缓冲区事件 回调函数

	bufferevent_enable(bufevent, EV_READ);										 //启用缓冲区事件 读功能
	
	//bufferevent_free(budevent);															 //释放缓冲区事件
}



int main(int argc,char *argv[])
{
	struct sockaddr_in addr_s;
	memset(&addr_s, 0, sizeof(addr_s));
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr.s_addr);

	
	struct event_base * base = event_base_new();		//创建底座


	struct evconnlistener * listener = evconnlistener_new_bind(base, cb_listener, base, 
																LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
																36,(struct sockaddr*)&addr_s, sizeof(addr_s));	//创建监听器
	
	event_base_dispatch(base);							//设置 循环
	
	evconnlistener_free(listener);						//释放 监听器

	event_base_free(base);								//释放 底座

	return 0;
}
