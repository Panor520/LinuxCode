//利用自定义函数库，可以让代码结构更清晰
#include "customsocket.h"

int main(int argc, char *argv[])
{
	if(argc != 3)
		err("parameter error.\nexample:./server 10.219.10.193 10001\n");

	int lfd;

	lfd = Socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));//这个地方要注意转换字节序，不转换不会报错，很难检查出来。
	inet_pton(AF_INET,argv[1],(void*)&addr_s.sin_addr);
	//addr_s.sin_addr.s_addr = htonl(INADDR_ANY);  
		
	bind(lfd,(struct sockaddr *)&addr_s,sizeof(addr_s));
	
	Listen(lfd,128);

	int connfd;
	struct sockaddr_in  addr;
	socklen_t addr_len;

	connfd = Accept(lfd,(struct sockaddr *)&addr,&addr_len);
	
	char str_ip[50];
	printf("Connected client.ip:%s,port:%d \n",
			inet_ntop(AF_INET, &addr.sin_addr.s_addr, str_ip,sizeof(str_ip)), ntohs(addr.sin_port));
	
	char buf[1024];
	bzero(buf,0);//清空
	//read(connfd,buf,sizeof(buf));
	//readn(connfd,buf,4);
	readline(connfd,buf,1024);
	printf("read:%s",buf);
	//printf("\n");//当上面用readn时，这个地方的第一个\n总是失效的
	//write(STDOUT_FILENO,buf,strlen(buf));
			
	close(connfd);
	close(lfd);
	
	printf("close fd\n");

	return 0;
}
