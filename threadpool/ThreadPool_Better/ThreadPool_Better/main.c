#include "threadpool.h"

void TaskFunc(void* arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working,num = %d\n", pthread_self(), num);

    sleep(1);
}

//�����̳߳�
int main()
{

    threadpool_t* pool = threadpool_create(3, 10, 100);   //�����̳߳�
    for (int i = 0; i < 100; i++)
    {
        int* num = (int*)malloc(sizeof(int));
        *num = i + 100;                                 //ֻ���������Ե����ﴫ���ٶ���
        threadpool_add_task(pool, TaskFunc, num);
    }

    sleep(30);                                          //��֤�̳߳صĲ����̶߳�ִ������
    threadpool_destroy(pool);                           //�����̳߳أ�Ҫ��֤�̳߳صĲ����̶߳�ִ������
    return 0;
}