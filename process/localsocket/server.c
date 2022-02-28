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

int main(int argc, char*argv[])
{
	int fd, cfd, i, n;
	socklen_t len_s, len_c;
	char buf[1024];
	struct sockaddr_un addr_s, addr_c;
	memset(&addr_s, 0, sizeof(addr_s));
	
	addr_s.sun_family = AF_UNIX;
	strcpy(addr_s.sun_path, ADDR_SERVER);
	len_s = offsetof(struct sockaddr_un, sun_path) + strlen(ADDR_SERVER);
	
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	unlink(ADDR_CLIENT);
	bind(fd, (struct sockaddr*)&addr_s, len_s);
	
	listen(fd, 128);
	
	printf("Accept...\n");
	while(1)
	{
		len_c = sizeof(addr_c);

		cfd = accept(fd, (struct sockaddr*)&addr_c, &len_c);

		len_c -= offsetof(struct sockaddr_un, sun_path);

		addr_c.sun_path[len_c] = '\0';
		printf("client bind filename %s\n",addr_c.sun_path);
		
		while(1)
		{
			n = read(cfd,buf,sizeof(buf));
			if(n > 0)
			{
				for(i = 0; i < n; i++)
					buf[i] = toupper(buf[i]);

				write(cfd, buf, n);
			}
			else if(n == 0)
				break;
		}
		close(cfd);
	}
	
	close(fd);
	
	return 0;
}
