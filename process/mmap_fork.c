#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>    
#include <sys/wait.h>

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char * argv[])
{
	int openfd = open("./mmap_forktxt", O_CREAT | O_RDWR | O_TRUNC,0644);
	if(openfd == -1)
		err("open mmaptxtfork error\n");

	ftruncate(openfd,10);//对文件扩容10字节 bytes

	int len = lseek(openfd,0,SEEK_END);
	char * p = mmap(NULL,len,PROT_WRITE | PROT_READ,MAP_SHARED,openfd,0);
	if(p == MAP_FAILED)
	close(openfd);//创建完映射即可关闭文件。

	int pid = fork();

	if(pid == -1){
		err("fork error \n");
	}else if(pid >0){//父进程读共享内存区

		wait(NULL);
		printf("read:%s\n",p);
		int unp = munmap(p,len);
		if(unp == -1)
			err("munmap error\n");
		printf("parent process end..\n");

	}else if(pid == 0){//子进程写共享内存区
		
		strcpy(p,"123456789012345");
		printf("write:%s\n",p);
		printf("child process end..\n");
	
	}
}
