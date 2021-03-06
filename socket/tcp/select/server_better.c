#include "../lib/customsocket.h"

int main(int argc, char *argv[])
{
	if(argc!=3)
		err("parameters error.\nexample:./server_better 127.0.0.1 10001\n");

	int lfd, cfd, rfd;
	char ip_c[16];//为啥是16，百度吧
	char buf[1024];
	int client[FD_SETSIZE];//FD_SETSIZE 系统带的宏 值为1024
	int maxi;
	int sret, maxfd, i, j, nread;
	
	struct sockaddr_in addr_c,addr_s;
	socklen_t len_addr_c;
	bzero(&addr_c,sizeof(addr_c));
	bzero(&addr_s,sizeof(addr_s));
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr.s_addr);

	lfd = Socket(AF_INET, SOCK_STREAM, 0);//创建监听socket
	
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//设置端口可重用

	Bind(lfd, (struct sockaddr*)&addr_s, sizeof(addr_s));
	Listen(lfd,128);//设置同时连接客户端的最大值为128

	fd_set rset, allset;//rset用来表示传入传出集合，allset用来代表所有要被监听的文件描述符集合
	FD_ZERO(&rset);
	FD_ZERO(&allset);

	maxi = -1;//用来记录 client数组存在的最大值
	for(i = 0; i < FD_SETSIZE; i++)//将数组初始化
		client[i] = -1;

	maxfd = lfd;//设置最大为新创建的文件描述符
	FD_SET(lfd, &allset);//先将lfd，加入监听中
	

	while(1)
	{
		rset = allset;//始终将要监听的所有描述符作为传入参数

		sret = select(maxfd+1, &rset,NULL,NULL,NULL);//设置阻塞监听

		if(sret == -1)
			err("select error\n");
		else if(sret >0)//有满足读的描述符返回
		{
			if(FD_ISSET(lfd,&rset))//先处理lfd
			{
				len_addr_c = sizeof(addr_c);
				cfd = Accept(lfd, (struct sockaddr *)&addr_c, &len_addr_c);//建立连接 不会阻塞
				printf("client connected.ip:%s port:%d\n"
						,inet_ntop(AF_INET, &addr_c.sin_addr.s_addr, ip_c,sizeof(ip_c))
						,ntohs(addr_c.sin_port));
				
				for(i = 0; i < FD_SETSIZE; i++)//将新增的连接描述符存放到提升遍历效率的数组中***********
				{
					if(client[i] == -1)
					{
						client[i] = cfd;
						break;
					}
				}

					
				if(i == FD_SETSIZE)
				{
					fputs("client[] too many client\n",stderr);
					exit(1);
				}

				if(i > maxi)//保证 maxi 总是client数组中最大的下标
					maxi = i;

				FD_SET(cfd, &allset);//将新客户生成的cfd加入allset中

				if(maxfd < cfd)//让maxfd始终是最大的文件描述符
					maxfd = cfd;
								
				if(1 == sret)//如果返回的 sret=1且只有lfd，那就不用在执行下面处理客户端fd的代码了
					continue;
				
			}
			

			
			for(i = 0; i <= maxi; i++)//处理客户端连接的fd（客户端请求）
			{
				rfd = client[i];
				if(rfd == -1)//如果拿到的不是可用fd那就直接跳出本次循环
					continue;

				if(FD_ISSET(rfd, &rset))//遍历已存在的fd是否存在于select传出的rset集合中
				{
					bzero(buf, sizeof(buf));
					nread = read(rfd, buf, sizeof(buf));
					if(nread == 0)//表示对端关闭
					{
						close(rfd);
						FD_CLR(rfd,&allset);//将关闭的cfd从要监听的集合中去除
						client[i] = -1;//将关闭的cfd从数组中去除
					}
					else if(nread > 0)
					{
						for(j = 0; j < nread; j++)
							buf[j] = toupper(buf[j]);

						write(rfd, buf, nread);//向对端写数据
						printf("write to %s %d:%s\n"
								,ip_c
								,ntohs(addr_c.sin_port)
								,buf);
					}

					if(1 == sret)//如果传出的rset值为1，且上面逻辑已处理过了，就跳出for循环
						break;
				}
			}

		}
	}	

	close(lfd);

	return 0;
}
