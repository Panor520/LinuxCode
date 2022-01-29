//锁机制demo
//
//本demo有个小问题，sleep(1)可能导致一个线程争夺住CPU，而另一个线程一直不执行,要等一会就会执行到了。
//
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

pthread_mutex_t mylock;//定义互斥锁

void err(int errno,char * msg)
{
	fprintf(stderr,"%s error:%s\n",msg,strerror(errno));
	exit(-1);
}

void * tfunc(void * arg)
{
	while(1){
		pthread_mutex_lock(&mylock);//加锁
		printf("——hello ");
		usleep(1000);
		printf("world\n");
		pthread_mutex_unlock(&mylock);//解锁
	}
	return NULL;
}

int main()
{
	pthread_t tid;

	int pmi;
	pmi = pthread_mutex_init(&mylock,NULL);//初始化互斥锁，第二个参数默认NULL即可
	if(pmi != 0)
		err(pmi,"pthread_mutex_init");
	printf("%d\n",pmi);

	int pc;
	pc = pthread_create(&tid,NULL,tfunc,NULL);
	if(pc != 0)
		err(pc,"pthread_create");


	while(1){
		pthread_mutex_lock(&mylock);//加锁
		printf("——HELLO ");
		sleep(1);
		printf("WORLD\n");
		pthread_mutex_unlock(&mylock);//解锁
	}
	
	pthread_join(tid,NULL);

	pthread_mutex_destroy(&mylock);//销毁锁

	pthread_exit(0);
}
