#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h> 
#include <errno.h>

int main()
{
	int fd,fderr;

	fd = open("./test.txt",O_RDONLY);//read only,return descriptor
	fderr= open("./no.c",O_RDONLY);//test error

	printf("fd=%d ,errno=%d\n",fd,errno);
	printf("fderr=%d ,errno=%d\n",fderr,errno);

	int rfd = close(fd);//close descriptor
	printf("rfd=%d\n",rfd);

	return 0;
}
