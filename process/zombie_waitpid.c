#include <stdio.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	int wstatus;

	int ret = fork();
	if(ret == -1){
		perror("fork error\n");
		exit(0);
	}else if(ret == 0){
		
		printf("child process end,pid=%d\n",getpid());
		sleep(10);
		exit(111);
	}else if(ret > 0){
		
		int id= waitpid(ret,&wstatus,0);//阻塞等待
		//int id= waitpid(ret,&wstatus,WNOHANG);//非阻塞，会立即返回结果

		if(id == -1){
			perror("wait error\n");
			exit(0);
		}

		if(WIFEXITED(wstatus)){
		 	printf("exit normal %d \n", WEXITSTATUS(wstatus));
		}else if(WIFSIGNALED(wstatus)){
			//printf("exit by signal %d or coredump %d \n",WTERMSIG(wstatus),WCOREDUMP(wstatus));
			printf("exit by signal %d \n",WTERMSIG(wstatus));
		}else if(WIFSTOPPED(wstatus)){
			printf("stopped by signal %d or continue %d \n", WSTOPSIG(wstatus), WIFCONTINUED(wstatus));
		}

		printf("parent wait end,pid=%d id=%d\n",getpid(),id);
	}

	return 0;
}
