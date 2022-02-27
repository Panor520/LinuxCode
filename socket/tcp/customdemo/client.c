#include "customsocket.h"

int main(int argc,char *argv[])
{
	if(argc != 3)
		err("parameter error.\nexample:./client 127.0.0.1 10001\n");

	int fd;
	fd = Socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in addr_c;
	addr_c.sin_family = AF_INET;
	addr_c.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],(void *)&addr_c.sin_addr);	

	Connect(fd,(struct sockaddr *)&addr_c,sizeof(addr_c));

	char buf[1024];
	bzero(buf,0);
	strcpy(buf,"hello\nhaha");
	write(fd,buf,sizeof(buf));
	//write(fd,buf,3);
	printf("write:%s\n",buf);

	close(fd);

	return 0;
}
