#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define T_ "Try again\n"
#define O_ "Time Out\n"
int main()
{
	int fd;
	fd = open("/dev/tty",O_RDONLY | O_NONBLOCK);
	if(fd == -1){
		printf("open /dev/tty error!\n");
		exit(0);
	}
	printf("open /dev/tty ok...\n");
	
	char buf[100];
	int i,n;
	for(i=0;i<5;i++)
	{
		n = read(fd,buf,10);
		if(n>0)
			break;
		
		if(errno != EAGAIN){
			printf("read error~\n");
			exit(0);
		}
		else {
			write(STDOUT_FILENO,T_,10);
			sleep(2);
		}
	}

	if(i==5){
		write(STDOUT_FILENO,O_,10);
	}
	else{
		write(STDOUT_FILENO,buf,n);
	}
		
	close(fd);
	
	return 0;
}
