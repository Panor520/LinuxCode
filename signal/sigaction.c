#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

void catch(int sig)
{
	if(sig == SIGINT){
		printf("catch %d\n",sig);
	}else if(sig == SIGQUIT){
		printf("catch %d\n",sig);
	}

	return;
}

int main(int argc, char *argv[])
{

	struct sigaction act;
	act.sa_handler = catch;//指定捕捉函数
	sigemptyset(&(act.sa_mask));//将信号集清空，只在捕捉函数工作时有效。默认写法
	act.sa_flags = 0;//默认写法

	int ret = sigaction(SIGINT,&act,NULL);//ctrl+c
	if(ret == -1)
		err("sigaction SIGINT error\n");

	ret = sigaction(SIGQUIT,&act,NULL);//指定ctrl+、
	if(ret == -1)
		err("sigaction SIGQUIT error\n");
	
	while(1);

	return 0;
}
