#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 8080
#define MAX_BUFF 100 
int main()
{
    int sfd,asfd,pid;
    struct sockaddr_in my_addr, their_addr;
    int addr_len;
    char buff[MAX_BUFF];
    char msg[MAX_BUFF];
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
    
    if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind fail");
        exit(1);
    }
    
    if (listen(sfd, 20) == -1)
    {
        perror("listen fail");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr);
    asfd = accept(sfd, (struct sockaddr *) &their_addr, &addr_len);

    if (asfd == -1)
    {
        perror("accept fail");
    }

    //printf("%s:%d已经连接", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));

    //recv(asfd, buff, MAX_BUFF, 0);
    //printf("%s", buff);
    strcpy(msg, "message from server");
    send(asfd, msg, sizeof(msg), 0);
    //close(asfd);
    //close(sfd);
}
