#include <stdio.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	int ret = fork();
	if(ret == -1){
		perror("fork error\n");
		exit(0);
	}else if(ret == 0){
		
		sleep(6);
		printf("child process end\n");

	}else if(ret > 0){
		
		while(1){
			sleep(1);
			printf("parent process running\n");
		}

	}

	return 0;
}
