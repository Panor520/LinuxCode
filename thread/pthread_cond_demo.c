//条件变量 生产者消费者模型实例
//
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void err(int errno, char * msg)
{
	fprintf(stderr,"%s error:%s\n",msg,strerror(errno));
	exit(-1);//进程直接退出
}

struct mystruct{
	int num;
	struct mystruct * next;
	
};//链表作为共享数据

struct mystruct * head;//定义头指针

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;//静态初始化条件变量结构体

pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;//静态初始化互斥锁结构体



void * consumer(void * arg)
{
	struct mystruct * tmp;

	while(1){
		//利用互斥量进行消费
		pthread_mutex_lock(&mylock);//加锁

		while(head == NULL){//多个消费者时，这个地方就应该是while，避免其中一个消费者消费掉数据后，其他消费者却不知道的情况。
			pthread_cond_wait(&cond,&mylock);//头指针为空说明没有产品，需等待条件变量触发。//这行被触发时会重新加锁。
		}
		tmp = head;
		head = tmp->next;//链表头插法
		
		pthread_mutex_unlock(&mylock);
		printf("consumer %ld: %d \n",pthread_self(),tmp->num);
		free(tmp);

		usleep(rand()%5+1000000);
	}

	return NULL;
}

void * producer(void * arg)
{
	struct mystruct * tmp;
	while(1){
		tmp = malloc(sizeof(struct mystruct));
		tmp->num = rand()%1000 +1;
		printf("Producer : %d\n",tmp->num);

		//利用互斥量进行生产
		pthread_mutex_lock(&mylock);
		tmp->next = head;
		head = tmp;
		pthread_mutex_unlock(&mylock);
		
		pthread_cond_signal(&cond);//生产成功，唤醒消费者线程进行消费

		usleep(rand()%5+1000000);
	}
	
	return NULL;
}


int main()
{
	pthread_t p,c;

	pthread_mutex_init(&mylock,NULL);
	pthread_cond_init(&cond,NULL);
	
	int pc;
	pc = pthread_create(&p,NULL,producer,NULL);
	if(pc != 0)
		err(pc,"pthread_create producer");

	int pcc;
	pcc = pthread_create(&c,NULL,consumer,NULL);
	if(pcc != 0)
		err(pcc,"pthread_create consumer");
	
	pthread_join(p,NULL);
	pthread_join(c,NULL);

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mylock);
	pthread_exit(0);
}




