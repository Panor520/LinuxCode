#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
#include <errno.h>


int main(int argc,char * argv[])
{
	char buffer[1024];

	int rfd,wfd;
	rfd = open(argv[1],O_RDONLY);//read fd
	if(rfd == -1)
		printf("read open error!\n");
	wfd = open(argv[2],O_RDWR | O_CREAT | O_TRUNC, 0664);//write fd,if this file exist,then trunc it,else create this file 
	if(wfd == -1)
		printf("write open error!\n");

	int n=0;
	while((n=read(rfd,buffer,1024))!=0)
	{
		if(n == -1)
			printf("read error!\n");
		if(write(wfd,buffer,n) == -1)
			printf("write error!\n");

	}

	if(close(rfd) == -1)
		printf("close rfd error!\n");
	if(close(wfd) == -1)
		printf("close wfd error!\n");

	
	return 0;
}
