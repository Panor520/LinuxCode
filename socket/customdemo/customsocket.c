#include "customsocket.h"


void err(char * msg)
{
	perror(msg);
	exit(-1);
}


int Socket(int domain, int type, int protocol)
{
	int x;
	x = socket(domain,type,protocol);
	if(x == -1)
		err("socket error\n");

	return x;//success fd
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int x;
	x = bind(sockfd,addr,addrlen);
	if(x == -1)
		err("bind error\n");

	return x;//success 0
}

int Listen(int sockfd, int backlog)
{
	int x;
	x = listen(sockfd, backlog);
	if(x == -1)
		err("listen error\n");

	return x;//success 0
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int x;
	x = connect(sockfd,addr,addrlen);
	if(x == -1)
		err("connect error\n");

	return x;//success 0
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int x;
	x = accept(sockfd,addr,addrlen);
	if(x == -1)
		err("accept error\n");
	
	return x;//success fd
}

ssize_t Read(int fd, void *buf, size_t count)
{
	ssize_t x;
	x = read(fd, buf, count);
	if(x == -1)
		err("read error\n");

	return x;//success fd 
}

//读指定字节函数
ssize_t readn(int fd, void *buf, size_t n)
{
	size_t nleft = n;//还需读取字节数
	ssize_t nread = 0;//已读字节数
	unsigned char * ptr = (char *)buf; //指向缓冲区的指针

	while(nleft > 0){
		nread = read(fd,ptr,nleft);
		if(-1 == nread)
		{
			if(EINTR == errno)
			{
				nread = 0;
			}
			else
				return -1;
		}
		else if(nread == 0)
		{
			break;
		}
		
		nleft -= nread; //减去已读字节数
		ptr += nread;	//将已读缓冲区的数据累加上去
	}

	return n-nleft;
}

static ssize_t readch(int fd, char *ptr)
{
	static int	count = 0;
	static char*        read_ptr = 0;
	static char         read_buf[1024*4] = {0};
	if (count <= 0)
    {
		again:
			count = read(fd, read_buf, sizeof(read_buf));
			if (-1 == count)
				if (EINTR == errno)//读取中断要重新读取
					goto again;
				else
					return -1;
			else if (0 == count)
				return 0;
			
			read_ptr = read_buf;
	}
	
	count--;
	*ptr = *read_ptr++;

	return 1;
}

//读到缓冲区满或者 \n 返回
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t         i = 0;
	ssize_t         ret = 0;
	char            ch = '\0';
	char*           ptr = NULL;
	
	ptr = (char *)vptr;

	for (i = 1; i < maxlen; ++i)
	{
		ret = readch(fd, &ch);
		if (1 == ret)
		{
			*ptr++ = ch;
			if ('\n' == ch)
				break;
		}
		else if (0 == ret)
		{
			*ptr = 0;
			return i-1;
		}
		else
			return -1;
	}

	*ptr = 0;
	return i;
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t          nleft = n;  //writen函数还需要写的字节数
	ssize_t         nwrite = 0; //write函数本次向fd写的字节数
	const char*     ptr = vptr; //指向缓冲区的指针
	while (nleft > 0)
	{
		if ((nwrite = write(fd, ptr, nleft)) <= 0)
		{
			if (nwrite < 0 && EINTR == errno)
				nwrite = 0;
			else
				return -1;
		}

		nleft -= nwrite;
		ptr += nwrite;
	}

	return n;
}

