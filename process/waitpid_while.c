#include <stdio.h>
#include <stdlib.h>

void demo1()//wait指定pid
{
	int wstatus,ret;
	int wpid,specpid;
	int i;

	for(i=0;i<5;i++){
		ret = fork();
		if(ret == 0){//ban child process fork
			break;
		}
		if(i==2){
			specpid=ret;
			printf("specify pid =%d\n",specpid);
		}

	}

	if(i == 5){
		printf("before wait pid=%d\n",specpid);
		
		wpid = waitpid(specpid,wstatus,0);//阻塞指定child pid，该进程阻塞结束就会使主线程退出。		
		
		printf("parent wait end res=%d\n",wpid);

		sleep(3);//使主线程晚于子线程退出。
		printf("parent end\n");
	
	}else{
		sleep(i);
		printf("child process running %d\n",getpid());
	}
	
	return;
}

void demo2()//while等待所有进程结束,阻塞方式
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
		
		while( (wpid = waitpid(-1,&wstatus,0)) != -1 ){//阻塞指定任意child pid。	
			if(wpid == -1){//返回值-1,代表进程都执行完毕。
				perror("waitpid error\n");
				break;
			}

			printf("waitpid wpid=%d \n",wpid);
		}

		printf("parent process end\n");
	
	}else{
		sleep(i);
		printf("child process running %d\n",getpid());
	}	
}

void demo3()//while等待所有进程结束 非阻塞WNOHANG
{
	int wstatus,ret;
	int wpid,specpid;
	int i;

	for(i=0;i<5;i++){
		ret = fork();
		if(ret == 0){//ban child process fork
			break;
		}
	}

	if(i == 5){
		
		while( (wpid = waitpid(-1,&wstatus,WNOHANG)) != -1 ){//阻塞指定任意child pid。	!=-1代表子进程都回收完毕。
			if(wpid > 0){
				printf("waitpid wpid=%d \n",wpid);
			}else{
				sleep(2);
				continue;
			}
		}

		printf("parent process end\n");
	
	}else{
		sleep(i);
		printf("child process running %d\n",getpid());
	}
}

int main(int argc,char * argv[])
{
	//demo1();
	//demo2();
	demo3();
}


