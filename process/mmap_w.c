#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

struct student {
	int id;
	char name[256];
	int age;
};

void err(char * msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char * argv[])
{
	struct student stu={1,"lp",18};
	int openfd;
	struct student * p;
	int len;

	openfd = open("./mmap_wtxt",O_CREAT | O_RDWR | O_TRUNC, 0644);
	if(openfd == -1)
		err("open mmap_wtxt error\n");
	
	int t = ftruncate(openfd,sizeof(struct student));
	if( t== -1)//扩展文件大小
		err("ftruncate error\n");
	
	len = lseek(openfd,0,SEEK_END);//获取文件大小

	p = mmap(NULL,len,PROT_WRITE,MAP_SHARED,openfd,0);
	if(p == MAP_FAILED)
		err("create mmap error\n");
	close(openfd);//创建完映射区即可关闭文件

	while(1){
		memcpy(p,&stu,sizeof(struct student));
		sleep(1);
		stu.id++;
	}

	if(munmap(p,len) == -1)
		error("munmap error\n");

	return 1; 
}
