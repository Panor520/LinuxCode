//本练习：利用兄弟进程实现ls -l | wc -l的功能。
//
//将ls -l默认输出到标准输出，重定向到管道的写端
//将 wc -l默认从标准输入读，重定向到管道的读端。
//
//兄进程实现ls -l的输出重定向以及管道的读
//弟进程实现wc -l的输入重定向以及管道的写
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
	int i,ret;
	int pipefd[2];

	if(pipe(pipefd) == -1){
		perror("create pipe error\n");
		exit(-1);
	}

	for(i = 0; i < 2; i++ ){
		ret = fork();	
		if(ret == -1){
			perror("fork error \n");
			exit(-1);
		}

		if(ret == 0)
			break;
	}


	if(i == 2){
		close(pipefd[0]);//两个进程间通讯时，其他进程必须关闭管道
		close(pipefd[1]);//两个进程间通讯时，其他进程必须关闭管道

		wait(NULL);	
		wait(NULL);//两个子进程，回收就要执行两次

	}else if(i == 0){//兄进程
		
		close(pipefd[0]);//往管道写入ls -l数据
		dup2(pipefd[1],STDOUT_FILENO);
		if(execlp("ls","ls","-l",NULL) == -1){
			perror("execlp ls error\n");
			exit(-1);
		}

		//下面的三行不会执行，因为上面的 execlp 函数
		//close(pipefd[1]);
		//wait(NULL);//等待子进程退出
		//exit(0);
	}else if(i == 1){//弟进程

		close(pipefd[1]);//读，wc -l从管道读
		
		dup2(pipefd[0],STDIN_FILENO);//wc -l默认从标准输入读，重定向为从管道读
		if(execlp("wc","wc","-l",NULL) ==-1){
			perror("execlp wc error\n");
			exit(-1);
		}
		
		//下面两行都不会执行，因为上面的 execlp 函数
		//close(pipefd[0]);
		//exit(0);
	}

	return 0;
}
