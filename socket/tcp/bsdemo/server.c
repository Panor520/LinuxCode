#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


#define MAXSIZE 1024

//获取一行 \r\n结尾的数据。  http头每行以/r/n结尾
// 返回读取到的字符个数。-1 表示recv 出错
int get_line(int cfd, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    while(1)
    {
       if((i < size-1) && (c != '\n'))  				//最后一个为\n 时跳出读取循环， i < size-1 是因为 i总是会多加 1。
       {
            n = recv(cfd, &c, 1, 0);    				//每次读一个字符,flags设置为0，此时recv()函数读取tcp 缓冲区中的数据到buf中，并从tcp 缓冲区中移除已读取的数据
            if(n > 0)
            {
                if(c == '\r')   						//单独读到\r并不是行结尾
                {
                    n = recv(cfd, &c, 1, MSG_PEEK);    	//预读一个字符是否为\n  //flags设置为MSG_PEEK，仅仅是把tcp 缓冲区中的数据读取到buf中，没有把已读取的数据从tcp 缓冲区中移除，如果再次调用recv()函数仍然可以读到刚才读到的数据。
                    if((n > 0) && (c == '\n')) 
                    {
                        recv(cfd, &c, 1, 0);    		//读到\n 再次循环就会跳出读取，从缓冲区中读出\n字符
                    }
                    else
                    {
                        c = '\n';						//如果读到最后没读到\n就给最后的字符附上 \n
                    }
                }
                buf[i] = c;								//将读取到的字符存储到接收的内存区
                i++;									//i总是+1，用来指定接收的内存区buf的位置
            }
            else
            {
                c = '\n';								//如果读到最后没读到\n就给最后的字符附上 \n
            }
        }
        else 											//不满足条件就退出
            break;
    }
    
    buf[i] = '\0';										//始终给buf添加上结束标记
    
    if(-1 == n) 										//recv发生错误时 n=-1 ,此时还应返回读取到的字符个数
        i = n;
    
    return i;											//返回读取到的字符个数，发生错误时返回 -1
}

//获取文件类型，并返回相应的Content-Type类型
const char * get_file_type(const char *name)
{
	char * dot;
	dot = strrchr(name, '.');		//自右向左查找'.'字符， 如不存在返回 NULL
	
	if(dot == NULL)
		return "text/plain; charset=utf-8";
	if(strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
		return "text/html; charset=utf-8";
	if(strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
		return "image/jpeg; charset=utf-8";
	if(strcmp(dot, ".gif") == 0)
		return "image/gif; charset=utf-8";
	if(strcmp(dot, ".png") == 0)
		return "image/png; charset=utf-8";
	if(strcmp(dot, ".css") == 0)
		return "application/x-csi; charset=utf-8";
	if(strcmp(dot, ".avi") == 0)
		return "audio/basic; charset=utf-8";
	if(strcmp(dot, ".mp3") == 0)
		return "audio/mp3; charset=utf-8";		
	if(strcmp(dot, ".mp4") == 0)
		return "video/mpeg4; charset=utf-8";	
	
	return "text/plain; charset=utf-8";
}

//发送错误的response的html内容给请求者
void send_error(int fd, int status, char *title, char *text)
{
	char buf[4096] = {0};
	
	sprintf(buf, "HTTP1.1 %d %s\r\nContent-Type:text/html\r\nContent-Length:%dConnection:close\r\n\r\n", status, title, -1);
	sprintf(buf+strlen(buf), "<html><head><title>%d %s</title></head>\n",status, title );
	sprintf(buf+strlen(buf), "<body> <h4>%d %s</h4>\n",status, title );
	sprintf(buf+strlen(buf), "%s\n <hr>\n</body>\n</html>\n", text );
	
	
	send(fd, buf, strlen(buf), 0);
}

//16进制 转 10进制
int hexit(char c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
		
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	
	if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	
	return 0;
}

//加码
void encode_str(char *to, int tosize, const char *from)
{
	int tolen;
	
	for (tolen = 0; *from != '\0' && tolen+4 < tosize; ++from)
	{
		if(isalnum(*from) || strchr("/_.-~", *from) != (char*)0)
		{
			*to = *from;
			++to;
			++tolen;
		}
		else 
		{
			sprintf(to, "%%%02x", (int) *from & 0xff);
			to += 3;
			from += 3;
		}
	}
	*to = '\0';
}

//解码
void decode_str(char *to, char *from)
{
	for (; *from != '\0'; ++from, ++to)
	{
		if(from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
		{
			*to = hexit(from[1]) * 16 + hexit(from[2]);
			from += 2;
		}
		else 
		{
			*to = *from;
		}
	}
	
	*to = '\0';
}

//给browser的请求建立通信
void do_accept(int lfd, int epfd)
{
	char ip[16];
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len = sizeof(clt_addr);
	
	int cfd = accept(lfd, (struct sockaddr*)&clt_addr, &clt_addr_len);
	if(cfd == -1)
	{
		perror("accept error\n");
		exit(-1);	
	}
	
	printf("client %s %d connected.\n"
							,inet_ntop(AF_INET, &clt_addr.sin_addr.s_addr, ip,sizeof(ip))
							,ntohs(clt_addr.sin_port));
	
	//设置 cfd 非阻塞
	int flag = fcntl(cfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(cfd, F_SETFL, flag);
	
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;	//边沿非阻塞模式
	ev.data.fd = cfd;
	
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
	if(ret == -1)
	{
		perror("epoll_ctl add cfd error\n");
		exit(1);		
	}
	
}

//关闭browser的连接
void disconnect(int cfd, int epfd)
{
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
	if(ret == -1)
	{
		perror("epoll_ctl_del error\n");
		exit(-1);
	}
	
	close(cfd);
}

//拼接 http 响应头
void send_respond(int cfd, int no, char *disp,const char *type, int len)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	//Connection:close\r\n
	sprintf(buf, "HTTP/1.1 %d %s\r\nContent-Type:%s\r\nContent-Length:%d\r\n\r\n", no, disp, type, len);//Content-Length为-1 就是会自动计算,会导致传输一直在进行，直到断开连接。设置具体的值，当传够具体的值后连接连接就自动关了
	
	int ret = send(cfd, buf, strlen(buf), 0);
	if(ret == -1)
	{
		printf("send_response error.\n");
	}
}

//发送http response给browser的请求
//传入通信fd，要获取的文件名
void send_file(int cfd, const char * file)				
{
	int n, ret;
	char buf[4096];										//发送response的缓冲区
	
	int ffd = open(file, O_RDONLY);						//打开要读的文件
	if(ffd == -1)
	{
		perror("open error");
		send_error(cfd, 404, "Not Found", "No such file or direntry");	//如果打开文件失败，那就发送 打开失败response信息
		//perror("open error");
		//exit(-1);
	}
	
	while(1)												//循环读取打开文件的数据并发送 response
	{
		memset(buf, 0, sizeof(buf));
		n = read(ffd, buf, sizeof(buf));					//读取文件内容
		if(n == 0)											//读到文件结尾就跳出
			break;
		else if(n == -1) 
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)		//不是真的发送出错，非阻塞传输，缓冲区满等产生时，应接着尝试发送
			{
				printf("-----read EAGAIN-----\n"); 			//读大文件时会多次进到这里
				continue;
			}
			else if(errno == EINTR)
			{
				printf("-----read EINTR-----\n");			//不是真的发送出错，发送中收到信号出现中断，应接着发送
				continue; 
			}
			else 
			{
				printf("-----read error-----\n");
				break;
			}
		}
		
		ret = send(cfd, buf, n, 0); 						//发送文件内容到通信 fd 
		
		if(ret == -1)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)		//不是真的发送出错，非阻塞传输，缓冲区满等产生时，应接着尝试发送
			{
				printf("-----send_file EAGAIN-----\n"); 	//发送大文件时会多次进到这里,处理大文件时这里会有问题
				continue;
			}
			else if(errno == EINTR)
			{
				printf("-----send_file EINTR-----\n");		//不是真的发送出错，发送中收到信号出现中断，应接着发送
				continue; 
			}
			else 
			{
				printf("send error\n");
				//exit(-1);
				send_error(cfd, 500, "Internal Server Error", "Internal Server Error");	//处理真正的异常发送问题，response 回发出错
				break;
			}	
		}
		
		
	}
	
	close(ffd);											//文件发送完毕，关闭打开的文件fd
}

//发送http response给browser的请求
//传入通信fd，及目录名
void send_dir(int cfd, const char *dirname)				
{
	int i, ret;
	
	char buf[4096] = {0};								//开辟用来发送 response的内存
		//<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">
	sprintf(buf, "<html><head><title>目录名：%s</title></head>", dirname);
	sprintf(buf+strlen(buf), "<body><h1>当前目录：%s</h1></body><table>", dirname);
	
	char enstr[1024] = {0};
	char path[1024] = {0};
	
	struct dirent **ptr;								//目录项二级指针
	int num = scandir(dirname, &ptr, NULL, alphasort);	//读取目录文件，返回值为目录项
	
	for (i = 0; i<num; i++)								//循环目录项
	{
		char *name = ptr[i]->d_name;					//取出目录项的名字
		
		sprintf(path, "%s%s", dirname, name);			//拼接文件的完整路径
		printf("path = %s ========\n",path);
		
		struct stat st;
		stat(path, &st);								//获取文件名
		
		
		//encode_str(enstr, sizeof(enstr), name);			//编码生成 unicode 
		strcpy(enstr,name);
		
		if(S_ISREG(st.st_mode))							//普通文件拼接返回的body中的 response内容
		{
			sprintf(buf+strlen(buf)
					,"<tr><td><a href=\"%s\">%s </a></td><td>%ld</td></tr>"
					,enstr, name, (long)st.st_size);	
		}
		else if(S_ISDIR(st.st_mode))					//目录文件拼接返回的body中的 response内容
		{
			sprintf(buf+strlen(buf)
					,"<tr><td><a href=\"%s\">%s/</a></td><td>%ld</td></tr>"
					,enstr, name, (long)st.st_size);
		}
		
		ret = send(cfd, buf, strlen(buf), 0);			//发送拼接的内容返回给请求者
		if(ret == -1)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)							//非阻塞传输，缓冲区满等产生时，应接着尝试发送
			{
				//printf("-----EAGAIN-----\n"); 	
				continue;
			}
			else if(errno == EINTR)						//发送中收到信号出现中断，应接着发送
			{
				//printf("-----EINTR-----\n");
				continue; 
			}
			else 
			{
				printf("send error\n");
				exit(-1);
			}	
		}
		
		memset(buf, 0, sizeof(buf));					//send完之后就可以清空 buf
	}
	
	sprintf(buf+strlen(buf), "</table></body></html>"); //拼接response完整的 html
	send(cfd, buf, strlen(buf), 0);					   	//发送 response
	
	printf("dir message send OK!\n");
}

//获取并处理browser的请求
void http_request(int cfd, const char * request)
{
	char method[16], path[256], protocol[16];
		
	sscanf(request, "%[^ ] %[^ ] %[^ ]", method, path, protocol);	//利用【正则表达式】获取 http 请求头的信息. GET /hello.c HTTP/1.1
		
	//printf("method=%s path=%s protocol=%s \n", method, path, protocol);	//解码前，打印出获取到的请求头的信息
	
	decode_str(path,path);								//解码请求的文件名  
	printf("method=%s path=%s protocol=%s \n", method, path, protocol);	//解码后，打印出获取到的请求头的信息
	
	char *file = path + 1;								//去掉path中的/ 获取访问文件名
	if(strcmp(path,"/") == 0)							//如果请求没指定文件，就是请求当前文件夹的内容
	{
		file = "./";
	}
	
	struct stat sbuf;
	int ret = stat(file, &sbuf);						//获取指定文件或当前文件夹的目录属性
	if(ret != 0)
	{
		printf("stat error.\n");
		send_error(cfd, 404, "Not Found", "No such file or direntry");	//没有指定文件的错误response
		//perror("stat error \n");
		//exit(-1);		
	}
	
	if(S_ISREG(sbuf.st_mode))							//如果文件属性为常规文件的response操作
	{
		send_respond(cfd, 200, "OK", get_file_type(file), sbuf.st_size);		//发送 http 响应头	
		
		send_file(cfd, file);							//发送 文件内容
	}
	else if (S_ISDIR(sbuf.st_mode))						//如果文件属性为目录的response操作
	{
		send_respond(cfd, 200, "OK", get_file_type(".html"), -1);			//发送 http 响应头	
		
		send_dir(cfd, file);								//处理要response的内容
	}
}

//读取 browser 发来的请求
void do_read(int cfd, int epfd)							
{
	char line[1024] = {0};								//用来读取的 内存区
	
	int len = get_line(cfd, line, sizeof(line));		//获取一行，读 http 请求协议 首行 GET /hello.c HTTP/1.1，http协议每行以\r\n 
	if(len == 0)										//表示browser端关闭连接
	{
		printf("client closed\n");
		disconnect(cfd, epfd);
	}
	else
	{
		///*
		while(1)										//必须读出所有的 http 头信息，否则后面对相应的fd发送请求总出问题
		{
			char buf[1024] = {0};
			len = get_line(cfd, buf, sizeof(buf));
			if(len == -1)
			{
				printf("get_line error.\n");			
				break;
			}
			/*else if(len == 0)							//读完 请求信息就跳出
			{
				printf("get_line break.\n");	
				break;
			}*/
			else
			{
				printf("buf:%s",buf);
			}
		}
		//*/
		
		//判断get请求
		if(strncasecmp(line, "GET", 3) == 0)
		{
			//处理http请求
			http_request(cfd, line);
			
			//处理完请求就关闭套接字，并加上favicon.ico图标就可以避免意料之外的错误
			disconnect(cfd, epfd);
		}
	}
	
}

int init_listen_fd(char * ip_s ,int port, int epfd)
{
	//创建监听套接字 lfd
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd == -1)
	{
		perror("socket error\n");
		exit(-1);
	}
	
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	//srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, ip_s, &srv_addr.sin_addr);
	
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	int ret = bind(lfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret == -1)
	{
		perror("bind error\n");
		exit(-1);
	}
	
	ret = listen(lfd, 128);
	if(ret == -1)
	{
		perror("listen error\n");
		exit(-1);
	}
	
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	if(ret == -1)
	{
		perror("epoll_ctl error\n");
		exit(-1);
	}
	
	return lfd;
}

void epoll_run(char * ip_s , int port)
{
    int i = 0;
    struct epoll_event all_events[MAXSIZE];
    
    //创建一个epoll监听树
    int epfd = epoll_create(MAXSIZE);
    if(epfd == -1)
    {
        perror("epoll_create error\n");
        exit(1);
    }
    
    //创建lfd， 并添加至监听数
    int lfd = init_listen_fd(ip_s, port, epfd);
    
    while(1)
    {
    	//监听事件节点对应事件
    	int ret = epoll_wait(epfd, all_events, MAXSIZE, -1);	//返回发生事件节点个数
    	if(ret == -1)
    	{
    		perror("epoll_wait error\n");
    		exit(-1);		
    	}
    	
    	
    	for (i=0; i<ret; i++)
    	{
    		struct epoll_event * pev = &all_events[i];
    		
    		if( !(pev->events & EPOLLIN))		//只处理读事件
    		{
    			continue;
    		}
    		
    		if(pev->data.fd == lfd)
    		{
    			do_accept(lfd, epfd);
    		}
    		else 
    		{
    			do_read(pev->data.fd, epfd);
    		}
    		
    	}
    }
}


void handle_pipe(int sig)
{
//do nothing
}
int ignore_signal()
{
    struct sigaction sa;
    sa.sa_handler = handle_pipe;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE,&sa,NULL);			//忽略掉SIGPIPE的信号，避免停掉服务
//do something
}

int main(int argc, char * argv[])
{
    if(argc < 4)                        	//指定参数运行程序
    {
    	printf("./a.out ip port path\n");
    	exit(-1);
    }
    
    ignore_signal();    					//屏蔽内核给的SIGPIPE信号 ,browser端和server端建立连接后就关闭了自己的写端，但读端开着。此时server send数据时内核会发出SIGPIPE的信号 并停掉服务进程，应设置屏蔽该SIGPIPE信号
        
    int port = atoi(argv[2]);           	//获取端口号
    
    int ret = chdir(argv[3]);           	//改变进程工作目录
    if(ret != 0)
    {
        perror("chdir error\n");
        exit(1);        
    }
    
    char ip_s[20];
    strcpy(ip_s, argv[1]);
    
    epoll_run(ip_s, port);                  //启动 epoll 监听
    
    return 0;
}