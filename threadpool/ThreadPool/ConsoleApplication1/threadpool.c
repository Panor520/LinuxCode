#include "threadpool.h"


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

		
		pthread_create(&pool->managerID, NULL, manager, NULL);			//���������߳�
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

void* worker(void* arg)
{
	ThreadPool* pool = (ThreadPool*)arg;

	while (1)
	{
		pthread_mutex_lock(&pool->mutexPool);
		while (pool->queueSize == 0 && pool->shutdown == 0)			//��ǰ�̶߳����Ƿ�Ϊ��
		{
			pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);	//���������߳�
		}

		if (&pool->shutdown)										//�̳߳ر��رվ���ֹ�����߳�
		{
			pthread_mutex_unlock(&pool->mutexPool);
			pthread_exit(NULL);
		}

		//�����������ȡ��һ������
		Task task;
		task.function = pool->taskQ[pool->queueFront].function;
		task.arg = pool->taskQ[pool->queueFront].arg;

		//�ƶ�ͷ���
		pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
		pool->queueSize--;

		
		pthread_mutex_unlock(&pool->mutexPool);						//����


		//æ�̹߳������ݿ���
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum++;
		pthread_mutex_unlock(&pool->mutexBusy);

		//ִ������
		task.function(task.arg);
		free(task.arg);												//�����ܱ�֤����������malloc�Ŀռ�Ĳ���
		task.arg = NULL;
		
		//æ�̹߳������ݿ���
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexBusy);
	}

	return NULL;
}
