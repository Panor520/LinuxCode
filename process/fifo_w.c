#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char * argv[])
{
	int fifofd,n=0;
	
	int fd = access("fifo.file",R_OK);//判断是否有管道文件
	if(fd == -1){
		perror("fifo.file access error\n");
		exit(-1);
	}

	if(fd != 0){//没有fifo管道文件就创建。

		if(mkfifo("fifo.file",0644) == -1){
			perror("mkfifo error\n");
			exit(-1);
		}
	}
	
	if((fifofd=open("fifo.file",O_RDWR | O_TRUNC)) == -1){//打开管道文件并清空文件
		perror("open fifo.file error\n");
		exit(-1);
	}

	char str[1024];
	bzero(str,sizeof(str));
	while(1){//持续向管道文件写入数据
		sprintf(str,"test fifo %d\n",n++);
		usleep(1000);
		write(fifofd,str,strlen(str));
	}

	close(fifofd);//

	return 0;
}
