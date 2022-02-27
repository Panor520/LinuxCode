#include "../lib/customsocket.h"

#define OPEN_SIZE 1024 //定义最大连接数

int main(int argc, char * argv[])
{
	if(argc != 3)
		err("parameter error.\nexample: ./server 127.0.0.1 10001\n");
	
	char buf[1024];
	char ip_c[16];
	int i, j, maxi;
	int lfd, cfd, rfd;
	struct sockaddr_in addr_s,addr_c;
	socklen_t len_addr_c;
	int rset, n;
	struct pollfd client[OPEN_SIZE];
	for(i = 0; i < OPEN_SIZE; i++)//将poll集合中所有fd均置为-1（即不存在）。
		client[i].fd = -1;

	bzero(&addr_c,sizeof(addr_c));
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr.s_addr);
	
	lfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	int opt =1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	Bind(lfd,(struct sockaddr*)&addr_s,sizeof(addr_s));
	Listen(lfd, 128);


	client[0].fd = lfd;//添加监听描述符到集合中
	client[0].events = POLLIN;//设置监听read事件
	maxi = 0;//lfd加进数组后，这个就要加1

	while(1)
	{
		rset = poll(client, maxi+1, -1);//-1 阻塞等待
		if(rset == -1)
			err("poll error\n");
		else if(rset > 0)
		{
			if(client[0].revents & POLLIN)//位与操作，判断是否发生事件
			{
				len_addr_c = sizeof(addr_c);
				cfd = Accept(lfd, (struct sockaddr*)&addr_c, &len_addr_c);

				printf("client %s %d connected.\n"
						,inet_ntop(AF_INET, &addr_c.sin_addr.s_addr, ip_c, sizeof(ip_c))
						,ntohs(addr_c.sin_port));
			
				for(i = 1; i < OPEN_SIZE; i++)
				{
					if(client[i].fd == -1)
					{
						client[i].fd = cfd;
						break;
					}
				}

				if(i == OPEN_SIZE)
					err("poll client[] too many clients.\n");
				
				client[i].events = POLLIN;

				if(i > maxi)//更新最大下标
					maxi = i;//i是从0开始的，maxi代表poll的监听个数

				if(--rset <= 0)
					continue;

			}
			
			
			for(i = 1; i <= maxi; i++)
			{
				rfd = client[i].fd;
				if(rfd < 0)
					continue;

				if(client[i].revents & POLLIN)//位与 发生的是不是 read事件
				{
					bzero(buf, sizeof(buf));
					n = read(rfd, buf, sizeof(buf));

					if(n == 0)//对端关闭
					{
						close(rfd);
						client[i].fd = -1;
						printf("client[%d] close connect.\n",i);
					}
					else if(n < 0)//出错
					{
						if(errno == ECONNRESET)//收到RST标志位，需要重新建立连接
						{
							printf("client[%d] aborted connection.\n",i);
							close(rfd);
							client[i].fd = -1;
						}
						else
							err("read error.\n");
					}
					else if(n > 0)
					{
						for(j = 0; j < n; j++)
							buf[j] = toupper(buf[j]);

						write(rfd, buf, n);
						printf("%s",buf);
					}

					if(--rset <= 0)//如果poll返回为1且为cfd，上面就执行过一次，不需要再循环了，直接跳出。
						break;
				}
			}

		}

	}
	
	close(lfd);
	return 0;
}
