#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

void func()
{
	kill(getpid(),SIGKILL);
}

int main(int argc, char *argv[])
{
	int res;
	struct itimerval ni;

	//signal(SIGVTALRM,func);

	ni.it_interval.tv_sec=0;
	ni.it_interval.tv_usec=0;

	ni.it_value.tv_sec=1;
	ni.it_value.tv_usec=0;
	

	res=setitimer(ITIMER_REAL,&ni,NULL);
	if(res == -1)
		perror("setitimer error\n");

	int i=0;
	while(1)
		printf("%d\n",i++);
	

	return 0;
}
