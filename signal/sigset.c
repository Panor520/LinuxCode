//sigset_t
//
//
//
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void printsig(sigset_t * set)//循环打印未决信号集的值
{
	int i=1;
	for(i;i<32;i++){
		if(sigismember(set,i) == 1)
			putchar('1');
		else
			putchar('0');
	}

	printf("\n");
}

int main(int argc, char *argv[])
{
	sigset_t myset;//自定义信号集

	sigaddset(&myset,SIGINT);//将ctrl+c 信号加到自定义信号集中
	int ret = sigprocmask(SIG_BLOCK,&myset,NULL);//将自定义信号集位或至PCB上
	if(ret == -1){
		perror("sigprocmask error\n");
		exit(-1);
	}

	while(1){//这个循环用来测试接收信号后打印数据的变化
	
		sigset_t sigp;
		int sp = sigpending(&sigp);//获取未决信号集
		if(sp == -1){
			perror("sigpending error\n");
			exit(-1);
		}
		printsig(&sigp);
		
		sleep(2);
	}
	return 0;
}

