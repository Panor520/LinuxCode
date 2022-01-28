#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

void err(int errno,char *msg)
{
	fprintf(stderr,"%s error:%s\n",msg,strerror(errno));
	exit(-1);
}

void * tfunc(void * arg)
{

	printf("child thread %ld\n",pthread_self());

	return NULL;
}

int main(int argc, char * argv[])
{
	pthread_t tid;
	
	pthread_attr_t attr;//创建线程属性结构体
	pthread_attr_init(&attr);//初始化结构体
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);//设置线程属性结构体为 detach 状态
	


	int pc;
	pc = pthread_create(&tid,&attr,tfunc,NULL);//利用线程属性结构体创建线程
	if(pc != 0)
		err(pc,"pthread_create");
	
	pthread_attr_destroy(&attr);//在利用完线程属性结构体后销毁该结构体
	
	int pj;
	pj = pthread_join(tid,NULL);//用来测试上面的线程属性设置是否成功，若成功则会报错。
	if(pj != 0)
		err(pj,"pthread_join");

	printf("main thread %ld\n",pthread_self());

	pthread_exit(0);;
}
