//#pragma once
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct ThreadPool ThreadPool;
//�����̳߳ز���ʼ��
ThreadPool* ThreadPool_Create(int max, int min, int queueSize);

//�����̳߳�
int ThreadPool_Destroy(ThreadPool *pool);

//���̳߳��������
void ThreadPool_Add(ThreadPool* pool, void(*func)(void*), void* arg);

//��ȡ�̳߳��й������̵߳ĸ���
int ThreadPool_GetbusyNum(ThreadPool* pool);

//��ȡ�̳߳��л��ŵ��̵߳ĸ���
int ThreadPool_GetliveNum(ThreadPool* pool);

//���ƹ����̺߳���
void* worker(void *arg);

//�������̺߳���
void* manager(void* arg);

//�˳��߳�
void ThreadExit(ThreadPool *pool);

#endif