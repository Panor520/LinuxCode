#include "threadpool.h"


#define TD_ADDMAXNUM 2

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

		
		pthread_create(&pool->managerID, NULL, manager, pool);			//创建管理线程
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

int ThreadPool_Destroy(ThreadPool* pool)
{
	if (pool == NULL)
	{
		return -1;
	}

	
	pool->shutdown = 1;						//关闭线程池,不需要锁，是因为只有这个地方会使用这个变量
	pthread_join(pool->managerID, NULL);	//阻塞回收管理者线程

	for (int i = 0; i < pool->liveNum; i++)
	{
		pthread_cond_signal(&pool->notEmpty);		//诱导线程关闭
	}

	//释放内存
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
	if (pool->shutdown)								//如果设置线程池销毁，就直接退出此添加过程
	{
		pthread_mutex_unlock(&pool->mutexPool);
		return NULL;
	}

	//添加任务 到任务队列
	while (pool->queueSize == pool->queueCapacity && !pool->shutdown)
	{
		pthread_cond_wait(&pool->notFull, &pool->mutexPool);	//阻塞生产者线程
	}

	pool->taskQ[pool->queueRear].function = func;
	pool->taskQ[pool->queueRear].arg = arg;
	pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
	pool->queueSize++;

	pthread_cond_signal(&pool->notEmpty);						//唤醒消费者线程 消费新增的任务 
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
		while (pool->queueSize == 0 && pool->shutdown == 0)			//当前线程队列是否为空
		{
			pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);	//阻塞工作线程

			//判断是不是要销毁线程
			if (pool->exitNum > 0)
			{
				pool->exitNum--;									//每次不管是否真的销毁线程，这个数都应该减至0
				if (pool->liveNum > pool->minNum)
				{
					pool->liveNum--;
					pthread_mutex_unlock(&pool->mutexPool);
					ThreadExit(pool);								//退出线程
				}
			}
		}

		if (pool->shutdown)										//线程池被关闭就终止管理线程
		{
			pthread_mutex_unlock(&pool->mutexPool);
			ThreadExit(pool);										//退出线程
		}

		//从任务队列中取出一个任务
		Task task;
		task.function = pool->taskQ[pool->queueFront].function;
		task.arg = pool->taskQ[pool->queueFront].arg;

		//移动头结点
		pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
		pool->queueSize--;

		pthread_cond_signal(&pool->notFull);						//唤醒消费者线程
		pthread_mutex_unlock(&pool->mutexPool);						//解锁

		
		printf("thread %ld start working...\n", pthread_self());
		//忙线程共享数据控制
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum++;
		pthread_mutex_unlock(&pool->mutexBusy);

		
		//执行任务
		task.function(task.arg);
		free(task.arg);												//尽可能保证传进来的是malloc的空间的参数
		task.arg = NULL;
		
		printf("thread %ld end working...\n", pthread_self());

		//忙线程共享数据控制
		pthread_mutex_lock(&pool->mutexBusy);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexBusy);
	}

	return NULL;
}

void* manager(void* arg)
{
	ThreadPool* pool = (ThreadPool*)arg;

	//管理者线程按一定频率执行
	while ( !pool->shutdown)
	{
		//假设周期为3s
		sleep(3);

		//取出线程池中任务的数量和当前线程的数量
		pthread_mutex_lock(&pool->mutexPool);
		int queueSize = pool->queueSize;				//取出要执行的任务个数 
		int liveNum = pool->liveNum;					//取出当前线程池中已存在的线程的数量
		pthread_mutex_unlock(&pool->mutexPool);		

		pthread_mutex_lock(&pool->mutexBusy);
		int busyNum = pool->busyNum;					//取出正在执行任务的线程数量
		pthread_mutex_unlock(&pool->mutexBusy);

		//任务的个数>存活的线程个数 && 存活的线程数 < 最大线程数
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

		//销毁线程
		//忙的线程* 2 < 存活的线程数 && 存活的线程 > 最小线程数
		if (busyNum * 2 < liveNum && liveNum > pool->minNum)
		{
			pthread_mutex_lock(&pool->mutexPool);
			pool->exitNum = TD_ADDMAXNUM;
			pthread_mutex_unlock(&pool->mutexPool);

			//诱导存活的线程自杀
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
