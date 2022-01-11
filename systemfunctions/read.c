#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>


int main()
{
	int openfd;
	openfd = open("./test.txt",O_RDONLY);
	if(openfd==-1)
		printf("open fail!\n");

	ssize_t num;
	char str[10];
	num = read(openfd,str,100);
	if(num == -1)
		printf("read error\n");
	else
		printf("read num=%ld\n",num);

	if(close(openfd)==-1)
		printf("close error.\n");

	return 0;
}
