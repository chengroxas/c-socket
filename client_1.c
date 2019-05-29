#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 9999 
#define MAX_BUFF 100 
int main()
{
    int sfd;
    struct sockaddr_in their_addr;
    char buff[MAX_BUFF];
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sfd == -1)
    {
        perror("create fail:");
        exit(1);
    }
    
    memset(&their_addr, 0, sizeof(struct sockaddr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    their_addr.sin_port = htons(PORT);
    
    if (connect(sfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect fail:");
        exit(1);
    }
    
    recv(sfd, buff, MAX_BUFF, 0);
    printf("%s", buff);
    close(sfd);
}
