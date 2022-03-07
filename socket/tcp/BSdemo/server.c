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


void epoll_run(int port)
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
    int lfd = init_listen_fd(port, epfd);
    
    
}

int main(int argc, char * argv[])
{
    if(argc < 3)                        //指定参数运行程序
        printf("./a.out port path\n");
        
    int port = atoi(argv[1]);           //获取端口号
    
    int ret = chdir(argv[2]);           //改变进程工作目录
    if(ret != 0)
    {
        perror("chdir error\n");
        exit(1);        
    }
    
    epoll_run(port);                    //启动 epoll 监听
    
    return 0;
}


int main(int argc, char * argv[])
{
	if(argc != 3)
		err("parameter error.\nexample:./server 127.0.0.1 10001\n");
	
	int lfd, cfd, rfd;
	int i, j, n;
	char buf[1024];
	struct sockaddr_in addr_s, addr_c;
	socklen_t ip_c_len;
	char ip_c[16];
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr);
	
	lfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	Bind(lfd, (struct sockaddr*)&addr_s, sizeof(addr_s));
	Listen(lfd, 128);
	
	int root;
	root = epoll_create(1024);//根节点

	int ctl, sret;
	struct epoll_event tmp, ep[1024];
	tmp.events = EPOLLIN;
	tmp.data.fd = lfd;
	ctl = epoll_ctl(root, EPOLL_CTL_ADD, lfd,&tmp);
	
	while(1)
	{
		sret = epoll_wait(root, ep, 1024, -1);
		if(sret < 0 )
			err("epoll_wait error.\n");
		else if(sret > 0)
		{
			for(i = 0; i < sret; i++)
			{
				rfd = ep[i].data.fd;

				if(rfd == lfd)
				{
					ip_c_len = sizeof(ip_c);
					cfd = Accept(rfd, (struct sockaddr*)&addr_c, &ip_c_len);
					
					printf("client %s %d connected.\n"
							,inet_ntop(AF_INET, &addr_c.sin_addr.s_addr, ip_c,sizeof(ip_c))
							,ntohs(addr_c.sin_port));
					
					tmp.events = EPOLLIN;
					tmp.data.fd = cfd;
					epoll_ctl(root, EPOLL_CTL_ADD, cfd, &tmp);
				}
				else
				{
					bzero(buf, sizeof(buf));
					n =read(rfd, buf, sizeof(buf));

					if(n == 0)
					{
						close(rfd);
						epoll_ctl(root, EPOLL_CTL_DEL, rfd, NULL);//将关闭连接的fd结点从epoll红黑树中去掉
						printf("client %s %d close.\n"
								,ip_c
								,ntohs(addr_c.sin_port));
					}
					else if(n > 0)
					{
						for(j = 0; j < n; j++)
							buf[j] = toupper(buf[j]);//小写转大写

						write(rfd, buf, n);//回写给客户端
						write(STDOUT_FILENO, buf, n);//输出到屏幕
					}
				}

			}
		}
	}



	close(lfd);

	return 0;
}
