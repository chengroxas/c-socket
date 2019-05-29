#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#define PORT 9999
#define MAX_BUFF 21
#define BACKLOG 20
#define CFD_LEN 20
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
	typedef struct
	{
		char name[10];
		int cfd;
	}USER;
	USER user[10];
	int sfd, cfd;
	int max_sfd = 0, max_cfd = 0;
	int cfd_queue[CFD_LEN];
	int cfd_len = 0;
	int on = 1;
	int recv_size;
	struct sockaddr_in server_addr, client_addr;
	//可以理解为文件描述符集合
	fd_set sfd_set, cfd_set;
	struct timeval timeout;
	unsigned int addr_len;
	char buff[MAX_BUFF-1];
	char msg[MAX_BUFF-1];
	char str[100];
	int is_read;
	char *test_str;

	sfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sfd == -1)
	{
		perror("create fail:");
		exit(1);
	}

	//将数据置零
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	//127.0.0.1或者ip地址都可以
	server_addr.sin_addr.s_addr = INADDR_ANY;
	//sin_port是short int型，这里只能是host to net short
	server_addr.sin_port = htons(PORT);

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if(bind(sfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind fail:");
		exit(1);
	}

	if(listen(sfd, BACKLOG) == -1)
	{
		perror("listen fail:");
		exit(1);
	}
	//timeout
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	//使用前必须FD_ZERO
	FD_ZERO(&sfd_set);
	FD_ZERO(&cfd_set);

	FD_SET(sfd, &sfd_set);
	//select第一个参数
	max_sfd = sfd;

	//accept中用到addr_len
	addr_len = sizeof(struct sockaddr);
	//客户端连接的描述符必须置零
	memset(&cfd_queue, 0, sizeof(cfd_queue));

	while(1)
	{
        FD_ZERO(&sfd_set);
		FD_ZERO(&cfd_set);
		FD_SET(sfd, &sfd_set);

		switch(select(max_sfd + 1, &sfd_set, NULL, NULL, &timeout))
		{
			case -1:
				perror("select error");
				break;
			case 0:
				break;
		    default:
				if (FD_ISSET(sfd, &sfd_set))
				{
					cfd = accept(sfd, (struct sockaddr *) &client_addr, &addr_len);
					if (cfd == -1)
					{
						perror("accept fail");
						exit(1);
					}
					recv_size = recv(cfd, buff, MAX_BUFF, 0);
					//printf("%s:%s:%d已经连接\n", buff, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					sprintf(str, "%s已经加入聊天室\n", buff);
					printf("%s", str);
					for(int z = 0;z < cfd_len; z++)
					{
						send(cfd_queue[z], str, strlen(str), 0);
					}
					cfd_queue[cfd_len] = cfd;
					strcpy(user[cfd_len].name, buff);
					user[cfd_len].cfd = cfd;
					max_cfd = MAX(max_cfd, cfd);
					cfd_len++;
					memset(buff, 0, sizeof(buff));
				}
				break;
		}
		for(int i = 0; i < CFD_LEN; i++)
		{
			if(cfd_queue[i] != 0)
			{
				FD_SET(cfd_queue[i], &cfd_set);
			}
		}
		FD_ZERO(&sfd_set);
		FD_SET(STDIN_FILENO, &sfd_set);

		switch(select(STDIN_FILENO + 1, &sfd_set, NULL, NULL, &timeout))
		{
			case -1:
				perror("select error:");
				exit(1);
				break;
			case 0:
				break;
			default:
				if (FD_ISSET(STDIN_FILENO, &sfd_set))
				{
					fgets(msg, MAX_BUFF, stdin);
					for(int k = 0;k < cfd_len;k++)
					{
						if (FD_ISSET(cfd_queue[k], &cfd_set))
						{
							send(cfd_queue[k], msg, sizeof(msg), 0);
						}
					}
				}
			    break;
		}
		switch(select(max_cfd + 1, &cfd_set, NULL, NULL, &timeout))
		{
			case -1:
				perror("select error:");
				exit(1);
				break;
			case 0:
				break;
			default:
				for(int j = 0;j < cfd_len;j++)
				{
					if (FD_ISSET(cfd_queue[j], &cfd_set))
					{
                        recv_size = recv(cfd_queue[j], buff, MAX_BUFF, 0);                    
						//recv发生错误的时候
						if (recv_size == -1)
						{
							FD_CLR(cfd_queue[j], &cfd_set);
							cfd_queue[j] = 0;
						}
						//断开连接的时候
						else if (recv_size == 0)
						{
							FD_CLR(cfd_queue[j], &cfd_set);
							close(cfd_queue[j]);
							printf("%s已经离开聊天室\n", user[j].name); 
                            //printf("%s:%d已经断开连接\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
							cfd_queue[j] = 0;
						}
						else 
						{
							test_str = (char *)malloc(sizeof(char));
							strcat(test_str, user[j].name);
							strcat(test_str, "说:");
							strcat(test_str, buff);
							//printf("%s说:%s", user[j].name, buff);
							memset(buff, 0, MAX_BUFF);
							is_read = 1;
							if (recv_size < MAX_BUFF)
							{
								is_read = 0;
							}
							while(is_read)
							{
								recv_size = recv(cfd_queue[j], buff, MAX_BUFF, 0);
								strcat(test_str, buff);
								if (recv_size < MAX_BUFF)
								{
									is_read = 0;
								}
							}
							for(int z = 0;z < cfd_len; z++)
							{
								if (z != j)
								{
								    send(cfd_queue[z], test_str, strlen(test_str), 0);
								}
							}
							free(test_str);
						}
						memset(buff, 0, MAX_BUFF);
					}
				}
				break;
		}
		FD_ZERO(&sfd_set);
	}
}
