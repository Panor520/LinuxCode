//信号量实现生产者消费者模型
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM 5
int queue[NUM];//全局数组实现环形队列
sem_t blank_num,produce_num;

void err_no(int errno,char * msg)
{
	fprintf(stderr,"%s error: %s\n",msg,strerror(errno));
	exit(-1);
}

void err(char * msg)
{
	printf("error:%s\n",msg);
	exit(-1);
}



void * producer(void * arg)
{
	int i = 0;

	while(1){
		sem_wait(&blank_num);
		queue[i] = rand()%1000+1;
		printf("produce %d \n",queue[i]);
		sem_post(&produce_num);

		i = (i+1) % NUM;//循环
		sleep(rand()%1);
	}

	return NULL;
}


void * consumer(void * arg)
{
	int i=0;

	while(1){
		sem_wait(&produce_num);
		printf("consumer :%d\n",queue[i]);
		queue[i] = 0;
		sem_post(&blank_num);
		
		i = (i+1) % NUM;
		sleep( rand()%1 );
	}

	return NULL;
}


int main(int argc,char *argv[])
{
	pthread_t p,c;

	sem_init(&blank_num,0,NUM);//初始化空格子信号量为5，线程间同步
	sem_init(&produce_num,0,0);//初始化产品数为0，线程间共享

	int pc;
	pc = pthread_create(&p,NULL,producer,NULL);
	if(pc != 0)
		err_no(pc,"pthread_create producer");


	pc = pthread_create(&c,NULL,consumer,NULL);
	if(pc != 0)
		err_no(pc,"pthread_create consumer");
	

	int pj;
	pj = pthread_join(p,NULL);
	if(pj != 0)
		err_no(pj,"pthread_join producer");

	pj = pthread_join(c,NULL);
	if(pj != 0)
		err_no(pj,"pthread_join consumer");

	pthread_exit(0);
}
