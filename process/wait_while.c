#include <stdio.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	int wstatus,ret;
	int wpid;
	int i;

	for(i=0;i<5;i++){
		ret = fork();
		if(ret == 0){//ban child process fork
			break;
		}
	}

	if(i == 5){

		while(1){
			wpid = wait(&wstatus);
			if(wpid == -1){
				break;
			}
			printf("wait wpid=%d\n",wpid);			
		}

		//while( (wpid == wait(&wstatus)) !=-1 ){ //这个注释的写法得不到想要的结果。
		//	printf("don't wait pid %d\n",getpid());
		//	printf("wait wpid=%d\n",wpid);			
		//}

		printf("parent wait end\n");
	}else{
		sleep(i);
		printf("child process running %d\n",getpid());
	}

	return 0;
}
