#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int num;//全局变量，用来标识

pthread_rwlock_t mylock;//读写锁结构体

void err(int errno,char * msg)
{
	fprintf(stderr,"%s error:%s\n",msg,strerror(errno));
	exit(-1);
}

void * tfunc_w(void * arg)
{
	int i = (int)arg;
	int t;
	while(1){
		pthread_rwlock_wrlock(&mylock);
		t = num;
		
		usleep(150000);
		
		printf("--%d tfunc_w num = %d ++num = %d tfunc_w--\n", i,t,++num);
	
		pthread_rwlock_unlock(&mylock);
		usleep(100000);

	}

	return NULL;
}

void * tfunc_r(void * arg)
{
	int i = (int)arg; 
		
	while(1){
		pthread_rwlock_rdlock(&mylock);

		printf("--%d tfunc_r num = %d tfunc_r--\n",i,num);
	
		pthread_rwlock_unlock(&mylock);

		usleep(150000);
	}
	return NULL;
}

int main()
{
	pthread_rwlock_init(&mylock,NULL);

	pthread_t tid[8];
	int pc_w,i;
	for(i=0; i<3; i++){//write lock
		pc_w = pthread_create(&tid[i],NULL,tfunc_w,(void *)i);
		if(pc_w != 0)
			err(pc_w,"write pthead_create");

		
	}
	

	int pc_r;
	int x;
	for(x=0; x<5; x++){//read lock
		pc_r = pthread_create(&tid[x+3],NULL,tfunc_r,(void *)x);
		if(pc_r != 0)
			err(pc_r,"read pthread_create");
	}

	int j;
	for(j=8; j<8; j++){
		pthread_join(tid[j],NULL);
	}
	
	pthread_rwlock_destroy(&mylock);

	pthread_exit(0);
}
