#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
	int sfd;
	struct sockaddr_in server_addr;
	char msg[100];

	sfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(sfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));

	fgets(msg, 100, stdin);

	send(sfd, msg, 100, 0);

}
