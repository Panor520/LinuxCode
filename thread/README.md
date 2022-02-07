本文为线程相关DEMO


pthread.c 
		如何创建多线程DEMO

pthread_muti.c    
		多线程demo, 

pthread_join.c   
		阻塞线程demo

pthread_attr_init.c  
		利用线程属性直接设置线程分离

pthread_detach.c
		创建完线程再设置线程分离



pthread_cancel.c  
		线程取消demo



pthread_mutex.c  
		线程同步：互斥量demo（只允许但线程读或写 共享数据）

pthread_rwlock.c
		线程同步：读写锁（优化可多读，但写还是单线程）

pthread_cond_demo.c  
		线程同步：条件变量 生产者与消费者模型 demo  

sem_produce_consumer.c	
		线程同步：信号量  生产者与消费者模型 demo	


生产者或消费者模型：
		必须生产出来 才能消费，也是锁的一种思想。

