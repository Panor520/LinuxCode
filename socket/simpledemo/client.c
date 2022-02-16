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

	int conn;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));//端口号必须和服务器对应

	inet_pton(AF_INET,argv[1],&addr.sin_addr.s_addr);


	conn = connect(fd,(struct sockaddr*)&addr,sizeof(addr));
	if(conn == -1)
		err("connect error\n");

	//向socket写（发送）数据
	char buf[1024];
	strcpy(buf,"hello\n");
	ssize_t wd;
	wd = write(fd,buf,strlen(buf)+1);
	if(wd == -1)
		err("write error\n");
	printf("write:%s\n",buf);

	sleep(1);//这个sleep是为了避免服务端还没返回，下面的read就已经执行完了的尴尬局面。
	
	//读取服务端回写的数据
	ssize_t rd;
	memset(buf,0,sizeof(buf));
	rd = read(fd,buf,sizeof(buf));
	if(rd == -1)
		err("read error\n");
	
	printf("read:%s\n",buf);

	close(fd);
	printf("close.\n");

	exit(0);
}
