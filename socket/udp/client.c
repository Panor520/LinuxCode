#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char*argv[])
{
	int fd;
	int n;
	char buf[1024];

	struct sockaddr_in addr_s;
	socklen_t len_addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr_s.sin_addr.s_addr);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	while(1)
	{	
		memset(buf, 0, sizeof(buf));
		scanf("%s",buf);
		sendto(fd, buf, sizeof(buf), 0, (struct sockaddr*)&addr_s, sizeof(addr_s));
		
		memset(buf, 0, sizeof(buf));
		len_addr_s = sizeof(addr_s);
		n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&addr_s, &len_addr_s);
		if(n == 0)
		{
			printf("server close.");
			break;
		}
		else if(n > 0)
		{
			printf("%s\n", buf);
		}
	}

	close(fd);

	return 0;
}
