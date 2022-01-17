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
		
		//execlp("ls","ls","-l","-h",NULL);
		char *args[] = {"ls","-l","-h",NULL};
		execvp("ls",args);	
		
	}else if(ret > 0){//parent process
		sleep(1);
		printf("parent process end.\n");
	}

	return 0;
}
