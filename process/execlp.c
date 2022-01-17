#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	int ret = fork();
	if(ret == -1){
		perror("fork error\n");
		exit(0);
	}else if( ret == 0){//child process
		//execlp("ls","-l","-h",NULL); //错误写法
		
		//execlp("ls","ls","-l","-h",NULL); //demo1
		execlp("date","date",NULL);	//demo2
		
	}else if(ret > 0){//parent process
		sleep(1);
		printf("parent process end.\n");
	}

	return 0;
}
