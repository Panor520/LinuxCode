#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EVENTS 1024
#define BUFLEN	4096
#define SERV_PORT 10001


void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

/*描述就绪文件描述符信息*/

struct myevent_s{

		int fd;												//要监听的文件描述符
		int events;											//对应的监听事件
		void *arg;											//泛型参数
		void (* call_back)(int fd, int events, void *arg);	//回调函数
		int status;											//1:在红黑树上（监听）。0:不在（不监听）
		char buf[BUFLEN];									
		int len;
		long last_active;									//记录每次加入红黑树 g_efd 的时间
};

int g_efd;													//全局变量，保存epoll_create返回的文件描述符
struct myevents_s g_events[MAX_EVENTS+1];					//自定义结构体类型数组。 +1  -> listen fd


/*将结构体 myevent_s 成员变量 初始化*/

void  eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg)
{
	ev->fd = fd;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;		//默认不在红黑树上
	memset(ev->buf, 0, sizeof(ev->buf));
	ev->len = 0;
	ev->last_active = time(NULL);	//调用eventset的时间

	return;
}

/*想epoll 监听的红黑树 添加一个 文件描述符*/

void eventadd(int efd, int events, struct myevent_s *ev)
{
	struct epoll_event epv;
	memset(&epv, 0, sizeof(epv));
	int op;
	epv.data.ptr = ev;//使用上struct epoll_event 的 ptr参数。
	epv.events = ev->events = events;	//EPOLLIN 或 EPOLLOUT

	if(ev->status = 0)//若不在红黑树上就准备预加入数据
	{
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}
	if(epoll_ctl(efd, op, ev->fd, &epv) < 0)	//实际添加
		printf("event add failed [fd=%d],events[%d]\n", ev->fd, events);
	else
		printf("event add OK [fd=%d],op%d,events[%0X]\n",ev->fd, op, events);
	
	return;
}

void acceptconn(int lfd, int events, void *arg)
{
	struct sockaddr_in cin;
	socklen_t len = sizeof(cin);
	int cfd, i;

	if((cfd = accept(lfd, (struct sockaddr *)&cin, &len)) == -1)
	{
		if(errno != EAGAIN && errno != EINTR )
		{
			/*暂不做处理*/
		}
		printf("%s : accept,%s\n",__func__,strerror(errno) );
		return ;
	}

	do{
		for(i = 0; i < MAX_EVENTS; i++)
		{
			if(g_events[i].status == 0)
				break;
		}

		if(i == MAX_EVENTS)
		{
			printf("%s : max connect limit[%d]\n",__func__, MAX_EVENTS);
			break;
		}

		int flag = 0;
		if((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0 )
		{
			printf("%s :fcntl nonblocking failed,%s\n", __func__, strerror(errno));
			break;
		}

		eventset(&g_event[i], cfd, recvdata, &g_events[i]);
		eventadd(g_efd, EPOLLIN, &g_events[i]);
	}while(0);

	printf("new connect [%s:%d][time:%ld], pos[%d]\n"
			,inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);

	return;
}

/*创建 socket ,初始化lfd*/
void initlistensocket(int efd, short port)
{
	struct sockaddr_in sin;

	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(lfd, F_SETFL, O_NONBLOCK);//将socket设为非阻塞

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	bind(lfd, (struct sockaddr*)&sin, sizeof(sin));

	listen(lfd, 20);

	eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);

	eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

	return ;
}

int main(int argc, char *argv[])
{
	unsigned short port = SERV_PORT;

	if(argc == 2)
		port = atoi(argv[2]);

	g_efd = epoll_create(MAX_EVENTS+1);		//创建红黑树，返回给全局变量 g_efd
	if(g_efd <= 0)
		printf("create efd in %s err %s\n", __func__, strerror(errno));

	initlistensocket(g_fd, port);

	struct epoll_event events[MAX_EVENTS+1];//保存已经满足就绪事件的文件描述符组
	printf("server running:port[%d]\n", port);

	int checkpos = 0, i;
	while(1)
	{
		/*超时验证，每次测试100个连接，不测试listenfd 当客户端60秒内没有和服务器通信，则关闭该客户端*/
		long now = time(NULL);
		for(i = 0; i < 100; i++,checkpos++)
		{
			if(checkpos == MAX_EVENTS)
				checkpos == 0;
			if(g_events[checkpos],status != 1)//不在红黑树上 跳过
				continue;
			
			long duration = now - g_events[checkpos].last_active;	//客户端不活跃时间

			if(duration >= 60)
			{
				close(g_events[checkpos].fd);	//关闭与该客户端链接
				printf("[fd=%d] timeout\n", g_events[checkpos].fd);
				eventdel(g_efd, &g_events[checkpos]);	//将该客户端 从红黑树 g_efd 移除
			}
		}

		/*监听红黑树g_efd,将满足的事件的文件描述符加至events数组中，1秒没有事件满足，返回0*/
		int nfd = epoll_wait(g_efd, events, MAX_EVENTS+1, 1000);
		if(nfd < 0)
		{
			printf("epoll_wait error,exit\n");
			break;
		}
		
		for(i = 0; i < nfd; i++)
		{
			/*使用自定义结构体myevents_s类型指针，接收 联合体data的void * ptr成员*/
			struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;

			if((events[i].events & EPOLLIN) && (ev->events & EPOLLIN) )	 //读就绪事件
				ev->call_back(ev->fd, events[i].events, ev->arg);

			if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) //写就绪事件
				ev->call_back(ev->fd, events[i].events, ev->arg);

		}
	}


	return 0;

}
