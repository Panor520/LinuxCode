//#pragma once
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <pthread.h>
#include <stdlib.h>

typedef struct ThreadPool ThreadPool;
//�����̳߳ز���ʼ��
ThreadPool* ThreadPool_Create(int max, int min, int queueSize);

//�����̳߳�

//���̳߳��������

//��ȡ�̳߳��й������̵߳ĸ���

//��ȡ�̳߳��л��ŵ��̵߳ĸ���

//
void* worker(void *arg);
#endif