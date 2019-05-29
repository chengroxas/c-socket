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
#define MAX_BUFF 20
#define IP 10.11.1.230
void filterLine(char *str);
int main()
{
	int sfd, cfd;
	struct sockaddr_in server_addr;
	struct timeval timeout;
	char msg[MAX_BUFF];
	fd_set cfd_set;

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
	FD_SET(STDIN_FILENO, &cfd_set);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	while(1)
	{
		FD_ZERO(&cfd_set);
		FD_SET(STDIN_FILENO, &cfd_set);
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
				filterLine(msg);
				send(sfd, msg, sizeof(msg), 0);
				break;
		}
		FD_CLR(STDIN_FILENO, &cfd_set);
	}
}

void filterLine(char *str)
{
	int i = 0;
	while(str[i] != '\n')
	{
		i++;
	}
	str[i] = '\0';
}
