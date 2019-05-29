#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#define PORT 9999
#define MAX_BUFF 100
#define IP 10.11.1.230
int main()
{
	int sfd, cfd;
	struct sockaddr_in server_addr;
	struct timeval timeout;
	char msg[MAX_BUFF], buff[MAX_BUFF];
	fd_set cfd_set, sfd_set;
	int recv_size;

	sfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sfd == -1)
	{
		perror("create fail:");
		exit(1);
	}

    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("10.11.1.230");
	server_addr.sin_port = htons(PORT);

	if (connect(sfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("connect fail");
		exit(1);
	}

	FD_ZERO(&cfd_set);
	FD_ZERO(&sfd_set);
	FD_SET(STDIN_FILENO, &cfd_set);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	while(1)
	{
		FD_ZERO(&cfd_set);
		FD_ZERO(&sfd_set);
		FD_SET(STDIN_FILENO, &cfd_set);
		FD_SET(sfd, &sfd_set);
		switch(select(sfd + 1, &sfd_set, NULL, NULL, &timeout))
		{
			case -1:
				perror("select error:");
				break;
			case 0:
				break;
			default:
				if (FD_ISSET(sfd, &sfd_set))
				{
					recv_size = recv(sfd, buff, MAX_BUFF, 0);
					if (recv_size == 0)
					{
						printf("服务端已经断开连接");
						FD_CLR(sfd, &sfd_set);
						close(sfd);
						exit(1);
					}
					else if (recv_size == -1)
					{
						FD_CLR(sfd, &sfd_set);
						close(sfd);
						perror("recv:");
						exit(1);
					}
					else
					{
						printf("%s", buff);
						memset(buff, 0, MAX_BUFF);
					}
				}
				break;

		}
		switch(select(STDIN_FILENO + 1, &cfd_set, NULL, NULL, &timeout))
	    {	
			case -1:
				perror("select error:");
				exit(1);
				break;
			case 0:
				break;
			default:
				fgets(msg, MAX_BUFF, stdin);
				send(sfd, msg, (strlen(msg) + 1)* sizeof(char), 0);
				memset(buff, 0, MAX_BUFF);
				break;
		}
		FD_CLR(STDIN_FILENO, &cfd_set);
	}
}
