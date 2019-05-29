#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#define PORT 8088
#define MAX_BUFF 100 
int MAX(int a, int b)
{
	if (a > b)
	{
		return a;
	}
	else 
    {
		return b;
	}
}
int main()
{
    int sfd,asfd,pid;
    struct sockaddr_in my_addr, their_addr;
    int addr_len;
    char buff[MAX_BUFF];
    char msg[MAX_BUFF];
	int on = 1;
	int sfd_client[21];//用来记录客户端的连接符
	int client_num = 0;//记录客户端的个数
	fd_set serversfd, clientsfd;//select
	struct timeval timeout;
	int max_serverfd, max_clientfd;
	int i;

	memset(&sfd_client, 0, sizeof(sfd_client));
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sfd == -1)
    {
        perror("create fail");
        exit(1);
    }

    memset(&my_addr, 0, sizeof(struct sockaddr));

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    my_addr.sin_port = htons(PORT);

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind fail netstat -tunlp");
        exit(1);
    }
    
    if (listen(sfd, 20) == -1)
    {
        perror("listen fail");
        exit(1);
    }

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	FD_ZERO(&serversfd);//套接字集合清空掉
	FD_ZERO(&clientsfd);
	FD_SET(sfd, &serversfd);//将套接字描述符放到服务端套接字集合中
    max_serverfd = sfd;

    addr_len = sizeof(struct sockaddr);

	while(1)
	{
		FD_ZERO(&serversfd);
		FD_ZERO(&clientsfd);
		FD_SET(sfd, &serversfd);

		//select把所有的套接字都循环一遍，有io事件则进行处理，没有则继续循环
		switch(select(max_serverfd + 1, &serversfd, NULL, NULL, &timeout))
		{
			case -1:
				perror("select error");
				break;
		    case 0:
				break;
			default:
				if (FD_ISSET(sfd, &serversfd))
				{
					asfd = accept(sfd, (struct sockaddr *) &their_addr, &addr_len);
					if (asfd == -1)
					{
						perror("accept fail");
					}
					printf("%s:%d已经连接\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));
					sfd_client[client_num++] = asfd;
					max_clientfd = MAX(max_clientfd, asfd);
				}
				break;
		}

		//将客户端的连接描述符放到clientsfd中
		for(i = 0; i < 20; i++)
		{
			if (sfd_client[i] != 0)
			{
				FD_SET(sfd_client[i], &clientsfd);
			}
		}

		switch(select(max_clientfd + 1, &clientsfd, NULL, NULL, &timeout))
		{
			case -1:
				break;
			case 0:
				break;
			default:
				for (i = 0;i < client_num; i++)
				{
					if (FD_ISSET(sfd_client[i], &clientsfd))
					{
						if(recv(sfd_client[i], buff, MAX_BUFF, 0) == -1)
						{
							FD_CLR(sfd_client[i], &clientsfd);
							printf("%s:%d已经断开连接\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));
							sfd_client[i] = 0;
						}
						else
						{
							printf("%s\n", buff); 
							memset(buff, 0, MAX_BUFF);
						}
					}
				}
				break;
		}
	}
	close(asfd);
	close(sfd);
}
