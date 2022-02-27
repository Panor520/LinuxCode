//服务端demo：
//			将收到的数据转为大写后返回。
//输入示例:./server 10001
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>


void err(char * msg)
{
	printf("server %s",msg);
	exit(-1);
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("parameter error\nexample:./server port");
		exit(-1);
	}
	int fd;
	fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == -1)
		err("socket error\n");

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bd;
	bd = bind(fd,(struct sockaddr *)&addr,sizeof(addr));
	if(bd == -1)
		err("bind error\n");
	
	int lis;
	lis = listen(fd,30);
	if(lis == -1)
		err("listen error\n");

	int connfd;
	struct sockaddr_in addr_cli;
	socklen_t len_cli = sizeof(addr_cli);
	connfd = accept(fd,(struct sockaddr*)&addr_cli,&len_cli);
	if(connfd == -1)
		err("accept error\n");
	
	char cli_ip[1024];
	printf("client ip:%s port:%d \n",
			inet_ntop(AF_INET,&addr_cli.sin_addr.s_addr,cli_ip,sizeof(cli_ip)),
			ntohs(addr_cli.sin_port));

	char buf[1024];
	ssize_t rd,i;
	while(1){
		rd = read(connfd,buf,sizeof(buf));
		if(rd == -1)
			err("read error\n");
		else if(rd == 0)
			break;
		printf("read:%s\n",buf);
		
		for(i=0; i<rd; i++){
			buf[i]=toupper(buf[i]);
		}
	
		ssize_t wd;
		wd = write(connfd,buf,rd);
		if(wd == -1)
			err("write error\n");
		printf("write:%s\n",buf);
	}

	close(connfd);
	close(fd);
	printf("close.\n");

	exit(0);
}
