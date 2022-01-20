#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
	int wstatus;
	int pipefd[2];
	char buf[1024];

	if(pipe(pipefd) == -1){
		perror("create pipe error\n");
		exit(-1);
	}

	int ret = fork();

	if(ret == -1){
		perror("fork error \n");
		exit(-1);
	}else if(ret == 0){
		close(pipefd[1]);//子进程只管读，关闭写端
		while(read(pipefd[0],buf,1) > 0)
			write(STDOUT_FILENO,buf,1);

		write(STDOUT_FILENO,"\n",1);
		close(pipefd[0]);
		exit(0);
	}else if(ret > 0){

		close(pipefd[0]);//主进程只往管道写数据，关闭读端
		strcpy(buf,"hello world");
		write(pipefd[1],buf,strlen(buf));
		close(pipefd[1]);

		wait(NULL);//等待子进程退出
		exit(0);
	}

	return 0;
}
