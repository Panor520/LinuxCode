#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	printf("before fork 1\n");
	printf("before fork 2\n");
	printf("before fork 3\n");

	int ret=fork();
	if( ret == -1 ){
		perror("fork error\n");
		exit(0);
	}else if(ret == 0){
		printf("it's parent ret=%d\n",ret);
	}else{
		printf("it's child ret=%d\n",ret);
	}

	printf("code end\n");//会显示两次

	int pid=getpid();
	printf("pid=%d\n",pid);

	int ppid=getppid();
	printf("ppid=%d\n",ppid);
}
