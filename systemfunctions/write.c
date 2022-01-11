#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
#include <errno.h>

int main()
{
	int openfd;
	openfd = open("test.txt",O_WRONLY);
	if(openfd == -1)
		printf("open test.txt error.\n");

	char buffer[100]= "hello world!";
	ssize_t wrRetuVal;
	wrRetuVal = write(openfd,buffer,5);
	if(wrRetuVal == -1)
		printf("write error.\n");
	else
		printf("write:%s\n",buffer);

	if(close(openfd) == -1)
		printf("close error.\n");

	return 0;
}
