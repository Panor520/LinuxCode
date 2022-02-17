#include "../lib/customsocket.h"

struct s_info{
	struct sockaddr_in addr_c;
	int cfd;
};

void * fun_child(void * arg)
{
	struct s_info * tmp = (struct s_info *)arg;//获取client信息及已连接描述符
	
	//int cfd = (int)(long)arg;//获取cfd
	int i,ret;
	char buf[1024],len_ip[20];

	while(1)
	{	
		bzero(buf, sizeof(buf));
		//ret = read(cfd, buf, sizeof(buf));
		ret = read(tmp->cfd, buf, sizeof(buf));
		if(ret == 0)
		{
			break;
		}

		for(i=0; i < ret; i++)
			buf[i]=toupper(buf[i]);
		
		//write(cfd,buf,ret);
		write(tmp->cfd,buf,ret);
		
		printf("client %s %d:%s\n",
				inet_ntop(AF_INET, &(tmp->addr_c.sin_addr.s_addr), len_ip,sizeof(len_ip))
				,ntohs(tmp->addr_c.sin_port)	
				,buf);
	}
	
	close(tmp->cfd);
	printf("client %s %d:closed.\n",len_ip,ntohs(tmp->addr_c.sin_port) );
	free(tmp);
	

	pthread_exit(0);
}

int main(int argc, char * argv[])
{
	if(argc != 3)
		err("parameter error.\nexample:./server 127.0.0.1 10001\n");

	int lfd,cfd;
	lfd = Socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in addr, addr_c;
	socklen_t addr_c_len;
	
	bzero(&addr,sizeof(addr));
		
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&addr.sin_addr);

	Bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
	Listen(lfd,128);
	
	

	while(1)
	{
		struct s_info * info_c =malloc(sizeof(struct s_info));//利用结构体存放连接信息
		char ip_c[50];
		
		addr_c_len = sizeof(addr_c);
		cfd =accept(lfd, (struct sockaddr*)&addr_c, &addr_c_len);
		
		info_c->addr_c = addr_c;
		info_c->cfd = cfd;
	
		printf("client connected.ip:%s,port:%d\n",
				inet_ntop(AF_INET, &addr_c.sin_addr.s_addr, ip_c, sizeof(ip_c))
				, ntohs(addr_c.sin_port));
		
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		
		pthread_t pid;
		
		//pthread_Create(&pid,&attr,fun_child,(void*)(long)cfd);
		pthread_Create(&pid,&attr,fun_child,(void*)info_c);
		
		pthread_attr_destroy(&attr);	
	}

	

	return 0;
}
