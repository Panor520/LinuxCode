#include "../lib/customsocket.h"

#define OPEN_SIZE 1024 //定义最大连接数

int main(int argc, char * argv[])
{
	if(argc != 3)
		err("parameter error.\nexample: ./server 127.0.0.1 10001\n");
	
	char buf[1024];
	char ip_c[16];
	int i, maxi;
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
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr.s_addr ,sizeof(addr_s));
	
	lfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	int opt =1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	Bind(lfd,(struct sockaddr*)&addr_s,sizeof(addr_s));
	Listen(lfd, 128);


	client[0].fd = lfd;//添加监听描述符到集合中
	client[0].events = POLLIN;//设置监听read事件
	maxi = 1;//lfd加进数组后，这个就要加1

	while(1)
	{
		rset = poll(client, maxi, -1);//-1 阻塞等待
		if(rset == -1)
			err("poll error\n");
		else if(rset > 0)
		{
			if(client[0].revents & POLLIN)//位与操作，判断是否发生事件
			{
				len_addr_c = sizeof(addr_c);
				cfd = Accept(lfd, (struct sockaddr*)&addr_c, len_addr_c);

				printf("client %s %d connected.\n"
						,inet_ntop(AF_INET,ip_c,&addr.sin_addr.s_addr,sizeof(ip_c))
						,ntohs(addr_c.sin_port));
			
				for(i = 0; i < OPEN_SIZE; i++)
				{
					if(client[i].fd == -1)
					{
						client[i].fd = cfd;
						break;
					}
				}

				if(i > OPEN_SIZE)
					err("poll client[] too many clients.\n");
				
				client[i].events = POLLIN;

				if(i > maxi)
					maxi = i;

				if(1 == rset)
					continue;

			}
		}

	}



	return 0;
}
