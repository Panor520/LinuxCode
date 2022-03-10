//������������ģ��
//һ���̴߳�����Ʒ��һ���߳����Ѳ�Ʒ�����������������������������������̡�

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct stuff{											//��������Ϊ�����������������ѵĲ�Ʒ�ṹ�� �������			
	int num;
	struct stuff * next;
};

struct stuff * head;									//����ͷָ��

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;		//��������̬��ʼ�� ������
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;			//��������̬��ʼ�� �������� 

void * producer(void *arg)								//������
{
	struct stuff * tmp;									//���������ߴ�����������ݵ�����
	while(1)
	{
		tmp = malloc(sizeof(struct stuff));
		tmp->num = rand()%1000 +1;
		printf("--producer num =%d\n",tmp->num);				//��������
		
		pthread_mutex_lock(&lock);								//���������ȼ���
		tmp->next = head;										//ͷ�巨
		head = tmp; 											//ͷ���ʼ��Ϊ�������Ľڵ�
		pthread_mutex_unlock(&lock);							//������������������
		
		pthread_cond_signal(&cond);								//������ɺ��������� pthread_cond_wait
		
		usleep(100000);
	}
}

void * consumer(void *arg)								//������
{
	struct stuff * tmp;									//���������� �������ݵ�����
	while(1)
	{
		pthread_mutex_lock(&lock);						//
		while(head == NULL)								//���������ʱ������ط���Ӧ����while����������һ�����������ѵ����ݺ�����������ȴ��֪���������
		{
			pthread_cond_wait(&cond, &lock);			//ͷָ��Ϊ��˵��û�в�Ʒ����ȴ���������������//���б�����ʱ�����¼�����
		}
		tmp = head;										//����ͷ�ڵ�
		head = tmp->next;								//ʹ�������ȥ�� ���ĵ���ͷ�ڵ�
		pthread_mutex_unlock(&lock);					//ʹ���깲��������������
		printf("consumer num =%d\n",tmp->num);
		
		free(tmp);										//ģ�����ѵ�����
		
		usleep(200000);
	}
}

int main(int argc,char *argv[])
{
	
	pthread_t ret, p, c;
	ret = pthread_create(&p, NULL, producer, NULL);		//�����������߳�
	if(ret != 0)
		printf("pthread_create producer error\n");
	
	ret = pthread_create(&c, NULL, consumer, NULL);		//�����������߳�
	if(ret != 0)
		printf("pthread_create consumer error\n");
	
	pthread_join( p, NULL);
	pthread_join( c, NULL);
	
	pthread_cond_destroy(&cond);						//������������
	pthread_mutex_destroy(&lock);						//���ٻ�����
	
	return 1;
}