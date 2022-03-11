//生产者消费者模型
//一个线程创建产品，一个线程消费产品，利用锁和条件变量控制生产和消费流程。

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct stuff{											//创建假设为用来保存生产和消费的产品结构体 链表队列			
	int num;
	struct stuff * next;
};

struct stuff * head;									//定义头指针

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;		//创建并静态初始化 互斥锁
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;			//创建并静态初始化 条件变量 

void * producer(void *arg)								//生产者
{
	struct stuff * tmp;									//创建生产者存放生产的数据的容器
	while(1)
	{
		tmp = malloc(sizeof(struct stuff));
		tmp->num = rand()%1000 +1;
		printf("--producer num =%d\n",tmp->num);				//生产数据
		
		pthread_mutex_lock(&lock);								//生产数据先加锁
		tmp->next = head;										//头插法
		head = tmp; 											//头结点始终为新生产的节点
		pthread_mutex_unlock(&lock);							//生产完数据立即解锁
		
		pthread_cond_signal(&cond);								//生产完成后立即唤醒 pthread_cond_wait
		
		usleep(100000);
	}
}

void * consumer(void *arg)								//消费者
{
	struct stuff * tmp;									//创建消费者 消费数据的容器
	while(1)
	{
		pthread_mutex_lock(&lock);						//
		while(head == NULL)								//多个消费者时，这个地方就应该是while，避免其中一个消费者消费掉数据后，其他消费者却不知道的情况。
		{
			pthread_cond_wait(&cond, &lock);			//头指针为空说明没有产品，需等待条件变量触发。//这行被触发时会重新加锁。
		}
		tmp = head;										//消耗头节点
		head = tmp->next;								//使链表队列去掉 消耗掉的头节点
		pthread_mutex_unlock(&lock);					//使用完共享数据立即解锁
		printf("consumer num =%d\n",tmp->num);
		
		free(tmp);										//模拟消费掉数据
		
		usleep(200000);
	}
}

int main(int argc,char *argv[])
{
	
	pthread_t ret, p, c;
	ret = pthread_create(&p, NULL, producer, NULL);		//创建生产者线程
	if(ret != 0)
		printf("pthread_create producer error\n");
	
	ret = pthread_create(&c, NULL, consumer, NULL);		//创建消费者线程
	if(ret != 0)
		printf("pthread_create consumer error\n");
	
	pthread_join( p, NULL);
	pthread_join( c, NULL);
	
	pthread_cond_destroy(&cond);						//销毁条件变量
	pthread_mutex_destroy(&lock);						//销毁互斥锁
	
	return 1;
}