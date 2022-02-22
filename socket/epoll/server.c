#include "../lib/customsocket.h"

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
