#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char * argv[])
{
	int openfd;
	openfd = open("./mmaptxt",  O_CREAT | O_TRUNC | O_RDWR ,0644);//这个地方的文件读写权限一定要注意，否则下面写共享内存时会出问题
	if(openfd == -1)
		err("open mmp.txt error\n");
	
	lseek(openfd,10,SEEK_END);//对文件进行扩容
	write(openfd,"\0",1);//
	//lseek(openfd,0,SEEK_SET);

	int len=lseek(openfd,0,SEEK_END);//获取文件长度

	char * mp = NULL;
	mp = mmap(NULL,len,PROT_WRITE|PROT_READ,MAP_SHARED,openfd,0);//MAP_SHARED 写的时候会写入磁盘
	if(mp == MAP_FAILED)
		err("mmap error\n");
	
	strcpy(mp,"12345678901234");//对共享内存区写,如果对指针赋值超过实际开辟的内存大小，会自动截断数据
	printf("%s\n",mp);

	int unret = munmap(mp,len);
	if(unret == -1)
		err("munmap error \n");

	return 1;
}
