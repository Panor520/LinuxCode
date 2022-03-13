#include "threadpool.h"


//任务结构体
typedef  struct Task
{
	void (*function) (void* arg);		//回调函数
	void* arg;							//回调函数参数
}Task;

//线程池结构体
struct ThreadPool
{
	//任务队列
	Task* taskQ;
	int queueCapacity;					//线程池容量
	int queueSize;						//当前任务个数
	int queueFront;
	int queueRear;

	pthread_t managerID;				//管理者线程ID
	pthread_t* threadIDs;				//工作的线程ID
	int minNum;							//最小线程数量
	int maxNum;							//最大线程数量
	int busyNum;						//正在忙碌线程数量
	int liveNum;						//存活的线程数量
	int exitNum;						//要销毁的线程个数

	pthread_mutex_t mutexPool;			//互斥锁，管理线程使用
	pthread_mutex_t mutexBusy;			//互斥锁，所busyNum数量
	pthread_cond_t notFull;				//条件变量，任务队列是否满了
	pthread_cond_t notEmpty;			//条件变量，任务队列是否为空

	int shutdown;						//是否要销毁线程池，销毁为1， 反之为0.
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

		//初始化线程池参数
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
		pool->liveNum = min;		//和最小个数相同
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

		//初始化任务队列
		pool->taskQ = (Task*)malloc(sizeof(Task) * queueSize);
		pool->queueCapacity = queueSize;
		pool->queueSize = 0;
		pool->queueFront = 0;
		pool->queueRear = 0;
			
		pool->shutdown = 0;

		
		pthread_create(&pool->managerID, NULL, manager, NULL);			//创建管理线程
		for (int i = 0; i < min; i++)
		{
			pthread_create(&pool->threadIDs[i], NULL, worker, pool);	//创建工作线程
		}
		return pool;
	} while (0);

	//释放资源
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
		while (pool->queueSize == 0 && pool->shutdown == 0)			//当前线程队列是否为空
		{
			pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);	//阻塞工作线程
		}

		if (&pool->shutdown)										//线程池被关闭就终止管理线程
		{
			pthread_mutex_unlock(&pool->mutexPool);
			pthread_exit(NULL);
		}

		//从任务队列中取出一个任务
		Task task;
		task.function = pool->taskQ[pool->queueFront].function;
		task.arg = pool->taskQ[pool->queueFront].arg;

		//移动头结点
		pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
		pool->queueSize--;

		
		pthread_mutex_unlock(&pool->mutexPool);						//解锁


		//忙线程共享数据控制
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum++;
		pthread_mutex_unlock(&pool->mutexBusy);

		//执行任务
		task.function(task.arg);
		free(task.arg);												//尽可能保证传进来的是malloc的空间的参数
		task.arg = NULL;
		
		//忙线程共享数据控制
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexBusy);
	}

	return NULL;
}
