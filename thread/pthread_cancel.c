#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void err(int errno)
{
	printf(" %s \n",strerror(errno));
	exit(-1);
}

void * tfunc(void * arg)
{
	long int i;

	for(i=0; i <2000000000; i++);
	

//	pthread_testcancel();//利用注释，看是否在指定取消点取消掉进程。
	
	printf("after pthread_testcancel\n");
	
	for(i=0; i <2000000000; i++);

	return (void*)i;
}

int main(int argc, char *argv[])
{
	pthread_t id;

	long int  ret=0;
	
	int pc;
	pc =  pthread_create(&id,NULL,tfunc,NULL); 
	if(pc != 0)
		err(pc);

	int x;
	x= pthread_cancel(id); 
	if(x != 0)
		err(x);

	int pj;
	pj = 	pthread_join(id,&ret); 
	if(pj != 0)
		err(pj);
	
	printf("i=%ld , x=%d pj=%d\n",ret,x,pj);//i的结果是-1,代表子线程非正常死亡，被cancel就是非正常消亡:。


	pthread_exit(0);
}
