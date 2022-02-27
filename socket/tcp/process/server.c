//利用自定义函数库，可以让代码结构更清晰
#include "../lib/customsocket.h"

void catch_child(int signo)
{
	/*
	while(1)
	{
		if(waitpid(0,NULL,WNOHANG) == -1)//没有线程回收就退出
			break;
	}*/
	while(waitpid(0,NULL,WNOHANG)>0);

	return ;
}


int main(int argc, char *argv[])
{
	if(argc != 3)
		err("parameter error.\nexample:./server 127.0.0.1 10001\n");

	int lfd;

	lfd = Socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in addr_s;
	bzero(&addr_s, sizeof(addr_s));
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));//这个地方要注意转换字节序，不转换不会报错，很难检查出来。
	inet_pton(AF_INET,argv[1],(void*)&addr_s.sin_addr);
	//addr_s.sin_addr.s_addr = htonl(INADDR_ANY);  
		
	bind(lfd,(struct sockaddr *)&addr_s,sizeof(addr_s));
	
	Listen(lfd,128);

	int connfd;
	pid_t pid;
	struct sockaddr_in  addr;
	bzero(&addr, sizeof(addr));
	socklen_t addr_len = sizeof(addr);

	while(1)
	{
		connfd = Accept(lfd,(struct sockaddr *)&addr,&addr_len);
		
		pid = fork();
		if(pid == 0)//child process
		{
			close(lfd);//关闭子进程中的监听 fd

			char str_ip[50];
			printf("Connected client.ip:%s,port:%d \n",
					inet_ntop(AF_INET, &addr.sin_addr.s_addr, str_ip,sizeof(str_ip)), 
					ntohs(addr.sin_port));
	
			char buf[1024];
			int n,i;
			n = 0;
			while(1)
			{
				bzero(buf,0);//清空
				n = Read(connfd,buf,sizeof(buf));
				if(n == 0)
				{
					close(connfd);
					exit(1);
				}
				for(i=0;i<n; i++)
				{
					buf[i]=toupper(buf[i]);
				}

				write(connfd, buf, n);
				printf("%s\n",buf);
				//write(STDOUT_FILENO, buf, n);//这个打印会打印很多莫名其妙的数据
			}
			close(connfd);
		}
		else if(pid>0)//parent process
		{
			struct sigaction act;
			act.sa_handler = catch_child;//指定信号捕捉函数
			sigemptyset(&(act.sa_mask));//默认写法，将信号集清空
			act.sa_flags = 0;//默认写法
			
			int ret = sigaction(SIGCHLD,&act,NULL);
			if(ret == -1)
				err("sigaction error\n");

			close(connfd);//parent no communication
			continue;
		}
		else if(pid < 0)
		{
			err("fork error\n");
		}
	}

	
	return 0;
}
