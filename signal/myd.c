//创建守护进程示例
//利用ps ajx |grep myd 查看守护进程
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char * argv[])
{

	int pid ;
	pid = fork();//1.创建子进程
	if(pid >0){

		exit(0);//2.退出父进程

	}else if(pid == 0){
		
		if(setsid() == -1)//3.创建会话
			err("setsid error \n");
		
		if(chdir("/bin/") == -1)//4.改变工作目录
			err("chdir error \n");
		

		umask(0022);//5.设置掩码
		
		close(STDOUT_FILENO);//6.关闭文件描述符

		int fd;
		fd = open("/dev/null",O_RDWR);
		dup2(fd,STDOUT_FILENO);//重定向描述符
		dup2(fd,STDERR_FILENO);//重定向描述符


		while(1);//模拟守护进程代码
		
		exit(0);
	}



	return 0;
}

