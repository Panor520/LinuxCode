#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/stat.h>
 #include <fcntl.h>

#define MAXSIZE 2048

//获取一行 \r\n结尾的数据。   http头每行以/r/n结尾
int get_line(int cfd, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    while(1)
    {
       if((i < size-1) && (c != '\n'))  //最后一个为\n 时跳出读取循环， i < size-1 是因为 i总是会多加 1。
       {
            n = recv(cfd, &c, 1, 0);    //每次读一个字符,flags设置为0，此时recv()函数读取tcp 缓冲区中的数据到buf中，并从tcp 缓冲区中移除已读取的数据
            if(n > 0)
            {
                if(c == '\r')   //单独读到\r并不是行结尾
                {
                    n = recv(cfd, &c, 1, MSG_PEEK);    //预读一个字符是否为\n  //flags设置为MSG_PEEK，仅仅是把tcp 缓冲区中的数据读取到buf中，没有把已读取的数据从tcp 缓冲区中移除，如果再次调用recv()函数仍然可以读到刚才读到的数据。
                    if((n > 0) && (c == '\n')) 
                    {
                        recv(cfd, &c, 1, 0);    //读到\n 再次循环就会跳出读取
                    }
                    else
                    {
                        c = '\n';
                    }
                }
                buf[i] = c;
                i++;
            }
            else
            {
                c = '\n';
            }
        }
        else 
            break;
    }
    
    buf[i] = '\0';
    
    if(-1 == n) //recv发生错误时 n=-1 ,
        i = n;
    
    return i;
}

void do_accept(int lfd, int epfd)
{
	char ip[16];
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len = sizeof(clt_addr);
	
	int cfd = accept(lfd, (struct sockaddr*)&clt_addr, &clt_addr_len);
	if(cfd == -1)
	{
		perror("accept error\n");
		exit(-1);	
	}
	
	printf("client %s %d connected.\n"
							,inet_ntop(AF_INET, &clt_addr.sin_addr.s_addr, ip,sizeof(ip))
							,ntohs(clt_addr.sin_port));
	
	//设置 cfd 非阻塞
	int flag = fcntl(cfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(cfd, F_SETFL, flag);
	
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;	//边沿非阻塞模式
	ev.data.fd = cfd;
	
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
	if(ret == -1)
	{
		perror("epoll_ctl add cfd error\n");
		exit(1);		
	}
	
}


void disconnect(int cfd, int epfd)
{
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
	if(ret == -1)
	{
		perror("epoll_ctl_del error\n");
		exit(-1);
	}
	
	close(cfd);
}

void http_request(const char * file)
{
	struct stat sbuf;
	
	int ret = stat(file, &sbuf);
	if(ret != 0)
	{
		perror("stat error \n");
		exit(-1);		
	}
	
	if(S_ISREG(sbuf.st_mode))
	{
		printf("it's file!\n");		
	}
}

void do_read(int fd, int epfd)
{
	char line[1024] = {0};
	
	int len = get_line(fd, line, sizeof(line));	//读 http 请求协议 首行 GET /hello.c HTTP/1.1
	if(len == 0)
	{
		printf("client closed\n");
		disconnect(fd, epfd);
	}
	else 
	{
		char method[16], path[256], protocol[16];
		
		sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
		
		printf("method=%s path=%s protocol=%s \n", method, path, protocol);
		
		while(1)
		{
			char buf[1024] = {0};
			len = get_line(fd, buf, sizeof(buf));
			if(len == '\n')
			{
				break;
			}
			else if(len == -1) 
			{
				break;
			}
		}
		
		if(strncasecmp(method, "GET", 3) == 0)
		{
			char *file = path + 1;
			http_request(file);
		}
	}
	
}

int init_listen_fd(char * ip_s ,int port, int epfd)
{
	//创建监听套接字 lfd
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd == -1)
	{
		perror("socket error\n");
		exit(-1);
	}
	
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = port;
	//srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, ip_s, &srv_addr.sin_addr);
	
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	int ret = bind(lfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret == -1)
	{
		perror("bind error\n");
		exit(-1);
	}
	
	ret = listen(lfd, 128);
	if(ret == -1)
	{
		perror("listen error\n");
		exit(-1);
	}
	
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	if(ret == -1)
	{
		perror("epoll_ctl error\n");
		exit(-1);
	}
	
	return lfd;
}

void epoll_run(char * ip_s , int port)
{
    int i = 0;
    struct epoll_event all_events[MAXSIZE];
    
    //创建一个epoll监听树
    int epfd = epoll_create(MAXSIZE);
    if(epfd == -1)
    {
        perror("epoll_create error\n");
        exit(1);
    }
    
    //创建lfd， 并添加至监听数
    int lfd = init_listen_fd(ip_s, port, epfd);
    
    while(1)
    {
    	//监听事件节点对应事件
    	int ret = epoll_wait(epfd, all_events, MAXSIZE, -1);	//返回发生事件节点个数
    	if(ret == -1)
    	{
    		perror("epoll_wait error\n");
    		exit(-1);		
    	}
    	
    	for (i=0; i<ret; i++)
    	{
    		struct epoll_event * pev = &all_events[i];
    		
    		if(!(pev->data.fd & EPOLLIN))		//只处理读事件
    		{
    			continue;
    		}
    		
    		if(pev->data.fd == lfd)
    		{
    			do_accept(lfd, epfd);
    		}
    		else 
    		{
    			do_read(pev->data.fd, epfd);
    		}
    		
    	}
    }
}

int main(int argc, char * argv[])
{
    if(argc < 4)                        //指定参数运行程序
    {
    	printf("./a.out ip port path\n");
    	exit(-1);
    }
        
        
    int port = atoi(argv[2]);           //获取端口号
    
    int ret = chdir(argv[3]);           //改变进程工作目录
    if(ret != 0)
    {
        perror("chdir error\n");
        exit(1);        
    }
    
    char ip_s[20];
    strcpy(ip_s, argv[1]);
    
    epoll_run(ip_s, port);                    //启动 epoll 监听
    
    return 0;
}



