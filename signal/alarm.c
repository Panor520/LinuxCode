//利用alarm函数，查看1秒钟能打印出多少个数字
#include <stdio.h>
#include <unistd.h>


int main(int argc, char * argv[])
{
	int i=0;
	alarm(1);

	while(1)
		printf("%d\n",i++);
	
	return 0;
}
