查看本文件夹代码时应先浏览下文。

摘要：
	本文件夹所有代码为Linux系统编程——进程相关demo。

demo主要包含：
	fork创建多进程。
	exec函数族。
	僵尸进程。
	wait和waitpid解决僵尸进程。
	进程间通讯：
		管道demo：
			含有pipe字符的文件.
			含有fifo字符的文件.
		创建映射区demo：
			含有mmap字符的文件.
	practice（练习）。


详细：


	README.md      
		:不做赘述。

	makefile     
		：编译文件

	fork.c    
		：fork创建子进程demo

	fork_loop.c  
		:循环fork多个子进程

	execl.c
		:execl函数demo	

	execlp.c  
		：execlp函数使用方式demo

	execvp.c  
		:execvp函数demo

	zombie.c       
		：僵尸进程示例，执行程序后利用 ps aux|grep zombie 查看进程信息

	zombie_wait.c 
		：wait解决僵尸进程问题，以及宏函数获取子进程状态。 kill -9 pid

	zombie_waitpid.c
		：waipid解决僵尸进程问题demo

	wait_while.c     
		：循环回收多个子进程demo

	waitpid_while.c  
		：循环回收多个子进程

	pipe.c  
		:管道使用demo

	printPSdata.c  
		：打印 ps aux 命令到文件psdata.txt

	
	fifo_w.c 和 fifo_r.c  
		：第二种管道fifo demo，w为读端，r为写端。利用创建的fifo.file管道文件

	mmap.c
		:创建映射区简单示例。 利用 cat mmaptxt查看映射区对应的磁盘文件内容

	mmap_fork.c
		：利用映射区实现父子进程间通讯。 利用 cat mmap_forktxt 查看映射区对应的磁盘文件内容

	mmap_w.c 和 mmap_r.c 
		：利用映射区实现不同进程间通讯. 两个进程打开同一个文件建立映射区。必须先执行./mmap_w 在执行./mmap_r 才能实现通讯

practice:
	进程相关的练习题demo。

		
	printPSdata.c  利用open、dup2、execlp函数将ps aux命令结果写入文件printPSdata.txt中，利用 cat命令可查看结果


	pipe_practice1.c  
		：父子进程利用管道进行通讯示例：用代码实现当前文件夹下 ls -l | wc -l的功能。

	pipe_practice2.c  
		：兄弟进程利用管道进行通讯示例：用代码实现当前文件夹下 ls -l | wc -l的功能。
