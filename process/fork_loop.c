#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	int i=0;

	for(i;i<5;i++){
		int ret = fork();
		if(ret == 0)
			break;

	}
	if(i!=5)
		printf("pid=%d\n",getpid());

	return 0;
}
