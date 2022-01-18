exec开头的文件为exec函数族的DEMO。
		


fork开头的文件为多进程Demo.


printPSdata.c  利用open、dup2、execlp函数将ps aux命令结果写入文件printPSdata.txt中，利用 cat命令可查看结果


zombie.c   僵尸进程demo

zombie_wait.c wait解决僵尸进程问题，以及宏函数获取子进程状态。 kill -9 pid
