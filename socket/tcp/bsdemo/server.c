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



//获取一行 \r\n结尾的数据。   http头每行以/r/n结尾
int get_line(int cfd, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    while(1)
    {
       if((i < size-1) && (c != '\n'))  //最后一个为\n 时跳出读取循环， i < size-1 是因为 i总是会多加 1。
       {
            n = recv(cfd, &c, 1, 0);    //每次读一个字符,flags设置为0，此时recv()函数读取tcp 缓冲区中的数据到buf中，并从tcp 缓冲区中移除已读取的数据
            if(n > 0)
            {
                if(c == '\r')   //单独读到\r并不是行结尾
                {
                    n = recv(cfd, &c, 1, MSG_PEEK);    //预读一个字符是否为\n  //flags设置为MSG_PEEK，仅仅是把tcp 缓冲区中的数据读取到buf中，没有把已读取的数据从tcp 缓冲区中移除，如果再次调用recv()函数仍然可以读到刚才读到的数据。
                    if((n > 0) && (c == '\n')) 
                    {
                        recv(cfd, &c, 1, 0);    //读到\n 再次循环就会跳出读取
                    }
                    else
                    {
                        c = '\n';
                    }
                }
                buf[i] = c;
                i++;
            }
            else
            {
                c = '\n';
            }
        }
        else 
            break;
    }
    
    buf[i] = '\0';
    
    if(-1 == n) //recv发生错误时 n=-1 ,
        i = n;
    
    return i;
}

const char * get_file_type(const char *name)
{
	char * dot;
	dot = strrchr(name, '.');		//自右向左查找'.'字符， 如不存在返回 NULL
	
	if(dot == NULL)
		return "text/plain; charset=utf-8";
	if(strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
		return "text/html; charset=utf-8";
	if(strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
		return "image/jpeg";
	if(strcmp(dot, ".gif") == 0)
		return "image/gif";
	if(strcmp(dot, ".png") == 0)
		return "image/png";
	if(strcmp(dot, ".css") == 0)
		return "application/x-csi";
	if(strcmp(dot, ".avi") == 0)
		return "audio/basic";
	if(strcmp(dot, ".mp3") == 0)
		return "audio/mp3";		
	if(strcmp(dot, ".mp4") == 0)
		return "video/mpeg4";	
	
	return "text/plain; charset=utf-8";
}

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


void disconnect(int fd, int epfd)
{
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
	if(ret == -1)
	{
		perror("epoll_ctl_del error\n");
		exit(-1);
	}
	
	close(fd);
}

//拼接 http 响应头
void send_respond(int fd, int no, char *disp,const char *type, int len)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	
	sprintf(buf, "HTTP/1.1 %d %s\r\nContent-Type:%s\r\nContent-Length:%d\r\nConnection:close\r\n\r\n", no, disp, type, len);//Content-Length为-1 就是会自动计算,会导致传输一直在进行，直到断开连接。设置具体的值，当传够具体的值后连接连接就自动关了
	
	send(fd, buf, strlen(buf), 0);
	
}

void send_file(int fd, const char * file)
{
	int n, ret;
	char buf[1024];
	
	int ffd = open(file, O_RDONLY);
	if(fd == -1)
	{
		send_error(fd, 404, "Not Found", "No such file or direntry");
		//perror("open error");
		//exit(-1);
	}
	
	while(1)
	{
		n = read(ffd, buf, sizeof(buf));
		if(n <= 0)							//读到文件末尾
			break;
		
		ret = send(fd, buf, n, 0); 
		///*
		if(ret == -1)
		{
			if(errno == EAGAIN)
			{
				printf("-----EAGAIN-----"); 
				continue;
			}
			else if(errno == EINTR)
			{
				printf("-----EINTR-----");
				continue; 
			}
			else 
			{
				perror("send error\n");
				exit(-1);
			}	
		}
		//*/
		
	}
	
	close(ffd);
}

void send_dir(int fd, const char *dirname)
{
	int i, ret;
	
	char buf[4096] = {0};
	
	sprintf(buf, "<html><head><title>目录名：%s</title></head>", dirname);
	sprintf(buf+strlen(buf), "<body><h1>当前目录：%s</h1></body><table>", dirname);
	
	char enstr[1024] = {0};
	char path[1024] = {0};
	
	struct dirent **ptr;	//目录项二级指针
	int num = scandir(dirname, &ptr, NULL, alphasort);
	
	for (i = 0; i<num; i++)
	{
		char *name = ptr[i]->d_name;
		
		//拼接文件的完整路径
		sprintf(path, "%s%s", dirname, name);
		printf("path = %s ========\n",path);
		
		struct stat st;
		stat(path, &st);
		
		//编码生成 unicode 
		encode_str(enstr, sizeof(enstr), name);
		
		if(S_ISREG(st.st_mode))
		{
			sprintf(buf+strlen(buf)
					,"<tr><td><a href=\"%s\">%s </a></td><td>%ld</td></tr>"
					,enstr, name, (long)st.st_size);	
		}
		else if(S_ISDIR(st.st_mode))
		{
			sprintf(buf+strlen(buf)
					,"<tr><td><a href=\"%s\">%s/</a></td><td>%ld</td></tr>"
					,enstr, name, (long)st.st_size);
		}
		
		ret = send(fd, buf, strlen(buf), 0);
		if(ret == -1)
		{
			if(errno == EAGAIN)
			{
				printf("-----EAGAIN-----"); 
				continue;
			}
			else if(errno == EINTR)
			{
				printf("-----EINTR-----");
				continue; 
			}
			else 
			{
				perror("send error\n");
				exit(-1);
			}	
		}
		
		memset(buf, 0, sizeof(buf));							//send完之后就可以清空buf
	}
	
	sprintf(buf+strlen(buf), "</table></body></html>");
	send(fd, buf, strlen(buf), 0);
	
	printf("dir message send OK!");
}

//void http_request(int fd, const char * file)
void http_request(int fd, const char * request)
{
	
	
	char method[16], path[256], protocol[16];
		
	sscanf(request, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
		
	printf("method=%s path=%s protocol=%s \n", method, path, protocol);
	
	decode_str(path,path);
	
	char *file = path + 1;	//去掉path中的/ 获取访问文件名
	if(strcmp(path,"/") == 0)
	{
		file = "./";		//资源目录当前位置
	}
	
	struct stat sbuf;
	int ret = stat(file, &sbuf);
	if(ret != 0)
	{
		send_error(fd, 404, "Not Found", "No such file or direntry");
		//perror("stat error \n");
		//exit(-1);		
	}
	
	if(S_ISREG(sbuf.st_mode))
	{
		//发送 http 响应头	
		//send_respond(fd, 200, "OK", "Content-Type: text/plain;  charset=utf-8", sbuf.st_size);
		send_respond(fd, 200, "OK", get_file_type(file), sbuf.st_size);
		
		//发送 文件内容
		send_file(fd, file);
	}
	else if (S_ISDIR(sbuf.st_mode))
	{
		send_respond(fd, 200, "OK", get_file_type(".html"), -1);
		
		send_dir(fd, file);
	}
}

void do_read(int fd, int epfd)
{
	char line[1024] = {0};
	
	int len = get_line(fd, line, sizeof(line));	//读 http 请求协议 首行 GET /hello.c HTTP/1.1
	if(len == 0)
	{
		printf("client closed\n");
		disconnect(fd, epfd);
	}
	else 
	{
		/*
		char method[16], path[256], protocol[16];
		
		sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
		
		printf("method=%s path=%s protocol=%s \n", method, path, protocol);
		*/
		
		while(1)
		{
			char buf[1024] = {0};
			len = get_line(fd, buf, sizeof(buf));
			if(len == '\n')
			{
				break;
			}
			else if(len == -1) 
			{
				break;
			}
		}
		
		//判断get请求
		if(strncasecmp(line, "GET", 3) == 0)
		{
			//char *file = path + 1;
			
			//处理http请求
			http_request(fd, line);
			
			//关闭套接字， cfd 从epoll上del。 http响应完应断开连接。
			disconnect(fd, epfd);
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

int main(int argc, char * argv[])
{
    if(argc < 4)                        //指定参数运行程序
    {
    	printf("./a.out ip port path\n");
    	exit(-1);
    }
        
        
    int port = atoi(argv[2]);           //获取端口号
    
    int ret = chdir(argv[3]);           //改变进程工作目录
    if(ret != 0)
    {
        perror("chdir error\n");
        exit(1);        
    }
    
    char ip_s[20];
    strcpy(ip_s, argv[1]);
    
    epoll_run(ip_s, port);                    //启动 epoll 监听
    
    return 0;
}