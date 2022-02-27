
#ifndef _CUSTOM_SOCKET_
#define _CUSTOM_SOCKET_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#include <pthread.h>

//自定义通讯函数,包裹报错信息
void err(char * msg);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

ssize_t Read(int fd, void *buf, size_t count);
ssize_t readn(int fd, void *buf, size_t count);
ssize_t readline(int fd, void *vptr, size_t maxlen);
ssize_t writen(int fd, const void *vptr, size_t n);

#endif
