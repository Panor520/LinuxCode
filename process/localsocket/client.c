#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>

#define ADDR_SERVER "test.server"
#define ADDR_CLIENT "test.client"

int main(int argc, char *argv[])
{
	int fd;
	char buf[1024];
	socklen_t len_c, len_s;
	struct sockaddr_un addr_c, addr_s;
	memset(&addr_c, 0, sizeof(addr_c));
	memset(&addr_s, 0, sizeof(addr_s));
	
	addr_c.sun_family = AF_UNIX;
	strcpy(addr_c.sun_path, ADDR_CLIENT);
	len_c = offsetof(struct sockaddr_un, sun_path) + strlen(addr_c.sun_path);
	
	unlink(ADDR_CLIENT);
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	bind(fd, (struct sockaddr*)&addr_c, len_c);
	
	addr_s.sun_family = AF_UNIX;
	strcpy(addr_s.sun_path, ADDR_SERVER);
	len_s = offsetof(struct sockaddr_un, sun_path) + strlen(addr_s.sun_path);
	connect(fd, (struct sockaddr*)&addr_s, len_s);

	while(1)
	{	
		memset(buf, 0, sizeof(buf));
		scanf("%s", buf);

		write(fd, buf, sizeof(buf));
		len_c =read(fd, buf, sizeof(buf));

		write(STDOUT_FILENO, buf, len_c);
	}
	
	close(fd);

	return 0;
}
