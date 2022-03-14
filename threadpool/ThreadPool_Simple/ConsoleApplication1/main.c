#include "threadpool.h"

void TaskFunc(void * arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working,num = %d\n", pthread_self(), num);

    sleep(1);
}

//测试线程池
int main()
{
    
    ThreadPool* pool = ThreadPool_Create(10, 3, 100);   //创建线程池
    for(int i= 0;  i< 100; i++)
    {
        int* num = (int*)malloc(sizeof(int));
        *num = i + 100;                                 //只是用来测试的这里传多少都行
        ThreadPool_Add(pool, TaskFunc, num);
    }

    sleep(30);                                          //保证线程池的测试线程都执行完了
    ThreadPool_Destroy(pool);                           //销毁线程池，要保证线程池的测试线程都执行完了
    return 0;
}