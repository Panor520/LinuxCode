#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc,char * argv[])
{
	int fifofd;
	
	int fd = access("fifo.file",R_OK);//判断是否有管道文件
	if(fd == -1){
		if(mkfifo("fifo.file",0644) == -1){
			perror("mkfifo error\n");
			exit(-1);
		}
	}

	fifofd = open("fifo.file",O_WRONLY);
	if(fifofd == -1){//打开管道文件并清空文件
		perror("open fifo.file error\n");
	}

	char str[1024];
	bzero(str,sizeof(str));

	int n;
	n=0;
	while(1){//持续向管道文件写入数据
		sprintf(str,"test fifo %d\n",n++);

		write(fifofd,str,strlen(str));
		sleep(1);
	}

	close(fifofd);//

	return 0;
}
