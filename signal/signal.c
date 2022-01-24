//ctrl+c 发出信号 会触发捕捉函数catch
//使用ctrl+\ 终止进程。
//
#include <stdio.h>
#include <signal.h>

void catch()//(int num)//这是signal函数第二个参数的固定写法,最多有一个int型参数
{
	int num;
	printf("catch %d\n",num);
}

int main(int argc, char * argv[])
{
	signal(SIGINT,catch);//指定捕捉的函数

	while(1);//等待获取信号

	return 0;
}
