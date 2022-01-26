#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
	int pid;

	pid = fork();

	if(pid > 0){

		sleep(20);
		return 0;
	}else if(pid == 0){
		printf("pid  %d\n",getpid());
		printf("pgid %d\n",getpgid(0));//参数0，代表前进程id，即getpid()的值。
		printf("psid %d\n",getsid(0));//参数0，代表当前进程id，即getpid()的值。
		
		
		setsid();//创建会话
		//创建会话后，进程ID、 组ID、会话id都是同一个。
		//
		printf("-----create session \n");
		printf("pid  %d\n",getpid());
		printf("pgid %d\n",getpgid(0));
		printf("psid %d\n",getsid(0));

		sleep(10);//利用ps ajx|grep session ，查看创建的会话进程
	}

}
