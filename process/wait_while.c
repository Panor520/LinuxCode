#include <stdio.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	int wstatus,ret;
	int res;
	int i;

	for(i=0;i<5;i++){
		ret = fork();
		if(ret == 0){//ban child process fork
			break;
		}
	}

	if(i == 5){
		sleep(0.5);
		while( (res == wait(&wstatus)) ){
			if(res > 0){
				printf("wait pid %d\n",res);
			}else{
				break;
			}
		}

		printf("parent wait end res=%d\n",res);
	}else{
		sleep(i);
		printf("child process running %d\n",getpid());
	}



	return 0;
}
