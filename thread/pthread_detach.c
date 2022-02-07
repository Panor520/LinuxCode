#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void err(int errno,char * msg)
{
	fprintf(stderr,"%s error:%s\n",msg,strerror(errno));
	exit(-1);
}

struct student{
	long int id;
	char name[20];
};

void * tfunc(void * arg)
{	
	sleep(1);
	struct student * s =(struct student *)arg;

	s->id=pthread_self();
	strcpy(s->name,"test");
	
	printf("id=%ld , name=%s\n",s->id,s->name);
	
	return NULL;
}

int main(int argc,char * argv[])
{
	pthread_t tid;
	struct student stu;
	
	int pcd;
	pcd = pthread_create(&tid,NULL,tfunc,&stu);
	if(pcd != 0)
		err(pcd,"pthread_create");
	
	int pdd;
	pdd = pthread_detach(tid);
	if(pdd != 0)
		err(pdd,"pthread_detach");

	//int pjd;
	//pjd = pthread_join(tid,NULL);//设置了pthread_detach的线程，不能再pthread_join，否则会报Invalid argument的错误。
	//if( pjd != 0)
		//err(pjd,"pthread_join");

	pthread_exit(0);
}
