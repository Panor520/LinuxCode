#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void * threadfunc(void * arg)
{
	printf("thread:thread id=%ld process id=%d\n",pthread_self(),getpid());
	
	return NULL;
}

int main(int argc,char * argv[])
{
	pthread_t id;
	
	printf("main:thread id=%ld process id=%d\n",pthread_self(),getpid());
	
	if(pthread_create(&id,NULL,threadfunc,NULL) != 0){
		perror("pthread_create error\n");
		exit(-1);
	}

	sleep(1);//避免主进程先消亡，而子线程还未运行
	return 0;
}
