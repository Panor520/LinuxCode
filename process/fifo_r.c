#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char * argv[])
{
	int fifofd,n;


	if((fifofd=open("fifo.file",O_RDONLY)) == -1){//打开fifo文件
		perror("open fifo.file error\n");
		exit(-1);
	}

	char str[1024];
	while(1){//持续读取fifo文件中数据
		if((n == read(fifofd,str,strlen(str))) == -1){
			perror("read error \n");
			exit(-1);
		}
	
		write(STDOUT_FILENO,str,n);//将读到的数据输出到标准输出
	
	}


	close(fifofd);

	return 0;
}
