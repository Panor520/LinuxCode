#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>


void err(char * msg)
{
	perror(msg);
	exit(-1);//结束当前进程
}

struct student{
	unsigned long int id;
	char name[20];
	int age;
}stu;

void * childfunc(void * arg)
{
	struct student * ret = (struct student *)arg;
	ret->id = pthread_self();
	strcpy(ret->name,"test");
	ret->age = 18;
	printf(" thread id=%ld,name=%s,age=%d \n",ret->id,ret->name,ret->age);
	return (void*)ret;
}

int main(int argc,char * argv[])
{
	struct student st;

	struct student  sta[5];
	pthread_t id[5]; 
	int i;
	
	for(i=0; i<5; i++){//循环生成多线程
		if(pthread_create(&id[i],NULL,childfunc,(void*)&st) != 0)
			err("pthread_create error\n");
		printf("id[%d]=%ld \n",i,id[i]);
	}

	int j;

	for(j=0; j<5; j++){//循环回收多线程
		
		if(pthread_join(id[j],(void**)&sta[j]) != 0)//传出参数也不能用一个，用一个会使打印的数据重复
			err("pthread_join error\n");
		
	}
	

	pthread_exit(NULL);
}
