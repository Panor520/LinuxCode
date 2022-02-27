/*udp实现demo*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	int lfd;
	int n, i;
	char buf[1024], ip[16];
	struct sockaddr_in addr, addr_c;
	socklen_t len_ip;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);
	
	lfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind(lfd, (struct sockaddr*)&addr, sizeof(addr));

	while(1)
	{	
		memset(buf, 0, sizeof(buf));
		len_ip = sizeof(addr_c);
		n = recvfrom(lfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr_c, &len_ip);
		if(n == 0)
		{
			printf("client close\n");
		}
		else if(n > 0)
		{	
			printf("recv %s %d:%s\n"
					,inet_ntop(AF_INET, &addr_c.sin_addr.s_addr, ip, sizeof(ip))
					,ntohs(addr.sin_port)
					,buf);
			for(i = 0; i < n; i++)
				buf[i] = toupper(buf[i]);

			sendto(lfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr_c, sizeof(addr_c));
		}

	}
	
	close(lfd);

	return 0;
}
