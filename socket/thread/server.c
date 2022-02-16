#include "../lib/customsocket.h"

int main(int argc, char * argv[])
{
	int lfd,connfd;
	lfd = Socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF+INET,argv[1],&addr.sin_addr);

	Bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
	Listen(lfd,128);

	while(1)
	{

	}



	return 0;
}
