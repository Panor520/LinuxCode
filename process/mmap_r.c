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
	struct student stu;
	int openfd;
	struct student * p;
	int len;

	openfd = open("./mmap_wtxt",O_RDONLY);
	if(openfd == -1)
		err("open mmap_wtxt error\n");	
	
	len = lseek(openfd,0,SEEK_END);//获取文件大小

	p = mmap(NULL,len,PROT_READ,MAP_SHARED,openfd,0);
	if(p == MAP_FAILED)
		err("create mmap error\n");
	close(openfd);//创建完映射区即可关闭文件

	while(1){
		printf("id=%d,name=%s,age=%d\n",p->id,p->name,p->age);
		sleep(1);
	}
	
	if(munmap(p,len) == -1)
		err("munmap error\n");

	return 1; 
}
