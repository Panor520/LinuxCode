本文件夹中所有内容都是线程池相关代码。

文件夹：
	本文件夹为VisualStudio连接Linux系统的demo实例。适合初学者思维，想要逻辑更清晰的看Better的代码较好。

文件介绍：
	主要的文件是 .c 和 .h 文件，其他文件是VS生成的项目文件。
		main.c
			线程池测试代码
		threadpool.h
			线程池头文件代码
		threadpool.c
			线程池实现代码

测试方法：
	1.VisualStudio调试
	2.Linux上直接测试
		build command:
			gcc  threadpool.c main.c -o  main.out -lpthread
		test command:
			./main.out
