本文件夹为 libevent 相关函数的demo



对应关系如下：
event_get_supported_methods.c:
	const char **event_get_supported_methods(void);	//查看支持哪些多路I/O，返回的是数组

event_base_get_method.c:
	const char * event_base_get_method(const struct event_base *base); //查看当前用的 多路I/O


build command:
	gcc event_get_supported_methods.c -o event_get_supported_methods -levent
	gcc event_base_get_method.c -o event_base_get_method -levent

test command:
	./event_get_supported_methods
	./event_base_get_method
