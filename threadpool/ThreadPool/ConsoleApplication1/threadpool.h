//#pragma once
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct ThreadPool ThreadPool;
//创建线程池并初始化
ThreadPool* ThreadPool_Create(int max, int min, int queueSize);

//销毁线程池
int ThreadPool_Destroy(ThreadPool *pool);

//给线程池添加任务
void ThreadPool_Add(ThreadPool* pool, void(*func)(void*), void* arg);

//获取线程池中工作的线程的个数
int ThreadPool_GetbusyNum(ThreadPool* pool);

//获取线程池中活着的线程的个数
int ThreadPool_GetliveNum(ThreadPool* pool);

//控制工作线程函数
void* worker(void *arg);

//管理者线程函数
void* manager(void* arg);

//退出线程
void ThreadExit(ThreadPool *pool);

#endif