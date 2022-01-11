#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define TRY_ "Try again\n"
#define TO_	 "Time out\n"

int main()
{
	int flag;
	flag = fcntl(STDIN_FILENO,F_GETFL);
	if(flag == -1){
			printf("FGETFL error!\n");
			exit(0);
	}

	flag |= O_NONBLOCK;//给获取到的flags加上非阻塞 //位与 涉及位图的概念
	
	int reset=fcntl(STDIN_FILENO,F_SETFL,flag);
	if(reset == -1){
		printf("FSETFL error!\n");
		exit(0);
	}
	int i=0,n;
	char buf[100];
	for(i;i<5;i++)
	{
		n = read(STDIN_FILENO,buf,10);
		if(n>0)
			break;
		else{
			write(STDOUT_FILENO,TRY_,10);
			sleep(2);
		}
	}
	
	if(i == 5){
		write(STDOUT_FILENO,TO_,10);
	}
	else{
		write(STDOUT_FILENO,buf,n);
	}
	
	return 0;
}
