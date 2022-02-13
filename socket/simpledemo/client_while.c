//客户端demo：
//		将输入的数据发送给客户端
//		测试时尽量写入字母数据
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void err(char * msg)
{
	printf("client %s",msg);
	exit(-1);
}

int main(int argc, char *argv[])
{
	if(argc !=3){
		printf("parameter error\nexample:./client ip port\n");
		exit(-1);
	}

	int fd;
	fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == -1)
		err("socket error\n");

	int connfd;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));//端口号必须和服务器对应

	inet_pton(AF_INET,argv[1],&addr.sin_addr.s_addr);


	connfd = connect(fd,(struct sockaddr*)&addr,sizeof(addr));
	if(connfd == -1)
		err("connect error\n");
	
	char buf[1024];
	
	while(1){
		scanf("%s",buf);//从屏幕输入数据

		ssize_t wd;
		wd = write(fd,buf,strlen(buf)+1);
		if(wd == -1)
			err("write error\n");
		
		sleep(1);//这个sleep是为了避免服务端还没返回，下面的read就已经执行完了的尴尬局面。
		
		ssize_t rd;
		memset(buf,0,sizeof(buf));
		rd = read(fd,buf,sizeof(buf));
		if(rd == -1)
			err("read error\n");
			
		printf("read:%s\n",buf);
	}

	close(connfd);
	close(fd);
	printf("close.\n");

	exit(0);
}
