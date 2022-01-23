#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char * argv[])
{
	int pid;
	int i;
	int specid;

	for(i=0; i<5; i++){
		
		pid = fork();
		if(pid == 0)
			break;

		if(i == 3)
			specid = pid;
	}


	if(i == 5){
		sleep(5);
		int k =	kill(specid,SIGINT);
		if(k == -1)
			err("kill error\n");
		printf("kill process %d success \n",specid);
		sleep(2);

		kill(0,SIGINT);
	}else{
		while(1){
			printf("child process %d \n",i);
			sleep(1);
		}
	
	}
	
}
