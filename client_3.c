#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 9999
#define MAX_BUFF 100 
void filterLine(char *str);
int main()
{
    int sfd, pid;
    struct sockaddr_in their_addr;
    char buff[MAX_BUFF], msg[MAX_BUFF];
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sfd == -1)
    {
        perror("create fail:");
        exit(1);
    }
    
    memset(&their_addr, 0, sizeof(struct sockaddr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_addr.s_addr = inet_addr("10.11.1.230");
    their_addr.sin_port = htons(PORT);
    
    if (connect(sfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect fail:");
        exit(1);
    }
    
	pid = fork();
	if (pid > 0)
	{
		while(1)
		{
			fgets(msg, MAX_BUFF, stdin);
			filterLine(msg);
			send(sfd, msg, sizeof(msg), 0);
		}
	}
	//else
	//{
	//	while(1)
	//	{
	//		recv(sfd, buff, MAX_BUFF, 0);
	//		printf("%s\n", buff);
	//	}
	//}
    close(sfd);
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
