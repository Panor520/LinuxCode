#include "threadpool.h"


#define TD_ADDMAXNUM 2

//����ṹ��
typedef  struct Task
{
	void (*function) (void* arg);		//�ص�����
	void* arg;							//�ص���������
}Task;

//�̳߳ؽṹ��
struct ThreadPool
{
	//�������
	Task* taskQ;
	int queueCapacity;					//�̳߳�����
	int queueSize;						//��ǰ�������
	int queueFront;
	int queueRear;

	pthread_t managerID;				//�������߳�ID
	pthread_t* threadIDs;				//�������߳�ID
	int minNum;							//��С�߳�����
	int maxNum;							//����߳�����
	int busyNum;						//����æµ�߳�����
	int liveNum;						//�����߳�����
	int exitNum;						//Ҫ���ٵ��̸߳���

	pthread_mutex_t mutexPool;			//�������������߳�ʹ��
	pthread_mutex_t mutexBusy;			//����������busyNum����
	pthread_cond_t notFull;				//������������������Ƿ�����
	pthread_cond_t notEmpty;			//������������������Ƿ�Ϊ��

	int shutdown;						//�Ƿ�Ҫ�����̳߳أ�����Ϊ1�� ��֮Ϊ0.
};


ThreadPool* ThreadPool_Create(int max, int min, int queueSize)
{
	
	ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
	do
	{
		if (pool == NULL)
		{
			printf("malloc ThreadPool error.\n");
			break;
		}

		//��ʼ���̳߳ز���
		pool->threadIDs = (pthread_t*)malloc(sizeof(pthread_t) * max);
		if (pool->threadIDs == NULL)
		{
			printf("malloc threadIDs error.\n");
			break;
		}

		memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
		pool->minNum = min;
		pool->maxNum = max;
		pool->busyNum = 0;
		pool->liveNum = min;		//����С������ͬ
		pool->exitNum = 0;	

		if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
			pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
			pthread_cond_init(&pool->notFull, NULL) != 0 ||
			pthread_cond_init(&pool->notEmpty, NULL) != 0
			)
		{
			printf("mutex or cond init error.\n");
			break;
		}

		//��ʼ���������
		pool->taskQ = (Task*)malloc(sizeof(Task) * queueSize);
		pool->queueCapacity = queueSize;
		pool->queueSize = 0;
		pool->queueFront = 0;
		pool->queueRear = 0;
			
		pool->shutdown = 0;

		
		pthread_create(&pool->managerID, NULL, manager, pool);			//���������߳�
		for (int i = 0; i < min; i++)
		{
			pthread_create(&pool->threadIDs[i], NULL, worker, pool);	//���������߳�
		}
		return pool;
	} while (0);

	//�ͷ���Դ
	if (pool && pool->threadIDs)
		free(pool->threadIDs);
	if (pool && pool->taskQ)
		free(pool->taskQ);
	if (pool)
		free(pool);
	return NULL;
}

int ThreadPool_Destroy(ThreadPool* pool)
{
	if (pool == NULL)
	{
		return -1;
	}

	
	pool->shutdown = 1;						//�ر��̳߳�,����Ҫ��������Ϊֻ������ط���ʹ���������
	pthread_join(pool->managerID, NULL);	//�������չ������߳�

	for (int i = 0; i < pool->liveNum; i++)
	{
		pthread_cond_signal(&pool->notEmpty);		//�յ��̹߳ر�
	}

	//�ͷ��ڴ�
	if (pool->taskQ)
		free(pool->taskQ);
	if (pool->threadIDs)
		free(pool->threadIDs);
	pthread_mutex_destroy(&pool->mutexBusy);
	pthread_mutex_destroy(&pool->mutexPool);
	pthread_cond_destroy(&pool->notEmpty);
	pthread_cond_destroy(&pool->notFull);

	free(pool);
	pool = NULL;

	return 0;
}

void ThreadPool_Add(ThreadPool* pool, void(*func)(void*), void* arg)
{
	pthread_mutex_lock(&pool->mutexPool);
	if (pool->shutdown)								//��������̳߳����٣���ֱ���˳�����ӹ���
	{
		pthread_mutex_unlock(&pool->mutexPool);
		return NULL;
	}

	//������� ���������
	while (pool->queueSize == pool->queueCapacity && !pool->shutdown)
	{
		pthread_cond_wait(&pool->notFull, &pool->mutexPool);	//�����������߳�
	}

	pool->taskQ[pool->queueRear].function = func;
	pool->taskQ[pool->queueRear].arg = arg;
	pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
	pool->queueSize++;

	pthread_cond_signal(&pool->notEmpty);						//�����������߳� �������������� 
	pthread_mutex_unlock(&pool->mutexPool);
}

int ThreadPool_GetbusyNum(ThreadPool* pool)
{
	pthread_mutex_lock(&pool->mutexBusy);
	int busyNum = pool->busyNum;
	pthread_mutex_unlock(&pool->mutexBusy);
	
	return busyNum;
}

int ThreadPool_GetliveNum(ThreadPool* pool)
{
	pthread_mutex_lock(&pool->mutexPool);
	int liveNum = pool->liveNum;
	pthread_mutex_unlock(&pool->mutexPool);

	return liveNum;
}

void* worker(void* arg)
{
	ThreadPool* pool = (ThreadPool*)arg;

	while (1)
	{
		pthread_mutex_lock(&pool->mutexPool);
		while (pool->queueSize == 0 && pool->shutdown == 0)			//��ǰ�̶߳����Ƿ�Ϊ��
		{
			pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);	//���������߳�

			//�ж��ǲ���Ҫ�����߳�
			if (pool->exitNum > 0)
			{
				pool->exitNum--;									//ÿ�β����Ƿ���������̣߳��������Ӧ�ü���0
				if (pool->liveNum > pool->minNum)
				{
					pool->liveNum--;
					pthread_mutex_unlock(&pool->mutexPool);
					ThreadExit(pool);								//�˳��߳�
				}
			}
		}

		if (pool->shutdown)										//�̳߳ر��رվ���ֹ�����߳�
		{
			pthread_mutex_unlock(&pool->mutexPool);
			ThreadExit(pool);										//�˳��߳�
		}

		//�����������ȡ��һ������
		Task task;
		task.function = pool->taskQ[pool->queueFront].function;
		task.arg = pool->taskQ[pool->queueFront].arg;

		//�ƶ�ͷ���
		pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
		pool->queueSize--;

		pthread_cond_signal(&pool->notFull);						//�����������߳�
		pthread_mutex_unlock(&pool->mutexPool);						//����

		
		printf("thread %ld start working...\n", pthread_self());
		//æ�̹߳������ݿ���
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum++;
		pthread_mutex_unlock(&pool->mutexBusy);

		
		//ִ������
		task.function(task.arg);
		free(task.arg);												//�����ܱ�֤����������malloc�Ŀռ�Ĳ���
		task.arg = NULL;
		
		printf("thread %ld end working...\n", pthread_self());

		//æ�̹߳������ݿ���
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexBusy);
	}

	return NULL;
}

void* manager(void* arg)
{
	ThreadPool* pool = (ThreadPool*)arg;

	//�������̰߳�һ��Ƶ��ִ��
	while ( !pool->shutdown)
	{
		//��������Ϊ3s
		sleep(3);

		//ȡ���̳߳�������������͵�ǰ�̵߳�����
		pthread_mutex_lock(&pool->mutexPool);
		int queueSize = pool->queueSize;				//ȡ��Ҫִ�е�������� 
		int liveNum = pool->liveNum;					//ȡ����ǰ�̳߳����Ѵ��ڵ��̵߳�����
		pthread_mutex_unlock(&pool->mutexPool);		

		pthread_mutex_lock(&pool->mutexBusy);
		int busyNum = pool->busyNum;					//ȡ������ִ��������߳�����
		pthread_mutex_unlock(&pool->mutexBusy);

		//����ĸ���>�����̸߳��� && �����߳��� < ����߳���
		if (queueSize > liveNum && liveNum < pool->maxNum)
		{
			pthread_mutex_lock(&pool->mutexPool);
			int counter = 0;
			for (int i = 0; i < pool->maxNum && counter < TD_ADDMAXNUM && pool->liveNum < pool->maxNum; i++)
			{
				if (pool->threadIDs[i] == 0)
				{
					pthread_create(&pool->threadIDs[i], NULL, worker, pool);
					counter++;
					pool->liveNum++;
				}
			}
			pthread_mutex_unlock(&pool->mutexPool);
		}

		//�����߳�
		//æ���߳�* 2 < �����߳��� && �����߳� > ��С�߳���
		if (busyNum * 2 < liveNum && liveNum > pool->minNum)
		{
			pthread_mutex_lock(&pool->mutexPool);
			pool->exitNum = TD_ADDMAXNUM;
			pthread_mutex_unlock(&pool->mutexPool);

			//�յ������߳���ɱ
			for (int i = 0; i < TD_ADDMAXNUM; i++)
			{
				pthread_cond_signal(&pool->notEmpty);
			}
		}
		
	}
	return NULL;
}

void ThreadExit(ThreadPool* pool)
{
	pthread_t tid = pthread_self();
	for (int i = 0; i < pool->maxNum; i++)
	{
		if (pool->threadIDs[i] == tid)
		{
			pool->threadIDs[i] = 0;
			printf("threadExit() called,%ld exiting...\n",tid);
			break;
		}
	}

	pthread_exit(NULL);
}
