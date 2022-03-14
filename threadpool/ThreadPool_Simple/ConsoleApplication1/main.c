#include "threadpool.h"

void TaskFunc(void * arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working,num = %d\n", pthread_self(), num);

    sleep(1);
}

//�����̳߳�
int main()
{
    
    ThreadPool* pool = ThreadPool_Create(10, 3, 100);   //�����̳߳�
    for(int i= 0;  i< 100; i++)
    {
        int* num = (int*)malloc(sizeof(int));
        *num = i + 100;                                 //ֻ���������Ե����ﴫ���ٶ���
        ThreadPool_Add(pool, TaskFunc, num);
    }

    sleep(30);                                          //��֤�̳߳صĲ����̶߳�ִ������
    ThreadPool_Destroy(pool);                           //�����̳߳أ�Ҫ��֤�̳߳صĲ����̶߳�ִ������
    return 0;
}