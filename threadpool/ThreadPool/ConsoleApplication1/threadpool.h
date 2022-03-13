//#pragma once
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <pthread.h>
#include <stdlib.h>

typedef struct ThreadPool ThreadPool;
//创建线程池并初始化
ThreadPool* ThreadPool_Create(int max, int min, int queueSize);

//销毁线程池

//给线程池添加任务

//获取线程池中工作的线程的个数

//获取线程池中活着的线程的个数

//
void* worker(void *arg);
#endif