#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	int fd;
	fd = open("./psdata.txt",O_RDWR|O_TRUNC|O_CREAT ,0666);
	if(fd == -1){
		perror("open ./psdata.txt error\n");
		exit(0);
	}
	
	int did = dup2(fd,STDOUT_FILENO);
	if(did == -1){
		perror("dup2 error\n");
		exit(0);
	}

	execlp("ps","ps","aux",NULL);

	return 0;
}
