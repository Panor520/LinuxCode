利用线程同步的互斥锁 和 条件变量 实现 生产者消费者模型的demo. 学习线程池的必备知识


文件介绍：
	ProducerConsumerModel.c
		一个线程创建产品，一个线程消费产品，利用锁和条件变量控制生产和消费流程。

bulid command:
	gcc ProducerConsumerModel.c -o ProducerConsumerModel.out -lpthread
	
test command:
	./ProducerConsumerModel