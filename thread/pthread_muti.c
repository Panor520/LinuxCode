#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void func()
{
	pthread_exit(0);
}

void * threadfunc(void * arg)
{
	int i = (int)arg;
	if(i == 2)//退出第三个线程
		func();

	printf("thread:thread id=%ld ，process id=%d  i=%d\n",pthread_self(),getpid(),i);
	
	return NULL;
}

int main(int argc,char * argv[])
{
	pthread_t id;
	int i;
	
	printf("main:thread id=%ld process id=%d\n",pthread_self(),getpid());
	
	for(i=0; i<5; i++){	
		if(pthread_create(&id,NULL,threadfunc,(void*)i) != 0){//这个地方传参注意，不要传地址（数据共享问题）进去，要穿值进去,否则回调函数得到的数据会有误。
			perror("pthread_create error\n");
			exit(-1);
		}

	}

	sleep(1);

	return 0;
}
