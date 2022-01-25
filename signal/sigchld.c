//指定SIGCHLD信号处理函数，当子进程结束时执行该函数


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

void catch(int signo)//signo是得到的信号整数值
{
	int  wstatus;
	int pid;
	while(1){//循环，避免多个子进程同一时间结束，而下面的打印只打印其中一个的问题。
		pid = waitpid(-1,&wstatus,NULL);//回收子进程
		if(pid == -1){
			break;
		}
		if( WIFEXITED(wstatus) )
			printf("catch process %d \n",WEXITSTATUS(wstatus));//正常回收子进程，就打印函数输出结果
	}

}

int main()
{
	int pid;
	int i;

	///
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGCHLD);
	sigprocmask(SIG_BLOCK,&set,NULL);
	///这一块设置阻塞子进程状态变化信号，避免子进程已结束但还父进程还没设置子进程结束信号处理函数。


	for(i=1; i<=15; i++){
		pid = fork();//循环fork 15个子进程
		if(pid == -1)
			err("fork  error\n");

		if(pid == 0)
			break;
	}

	if(i == 16){
		
		///
		struct sigaction act;
		act.sa_handler = catch;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		sigaction(SIGCHLD,&act,NULL);
		///注册子进程状态变化信号处理函数
		
		///
		sigprocmask(SIG_UNBLOCK,&set,NULL);
		///设置解除阻塞信号集，不解除上面注册的信号函数就不会执行。


	}else{//子进程执行代码

		printf("child process end  %d\n",getpid());
		return i;
	}




	
	return 0;
}
