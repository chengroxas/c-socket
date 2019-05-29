#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
	int sfd, cfd, nfds, n;
	struct sockaddr_in server_addr, client_addr;

	struct epoll_event ev, events[100];

	int epoll_fd;
	int epoll_size = 100;
	char buff[100];
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		perror("create fail:");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9999);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int on = 1;

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if (bind(sfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind fail:");
		exit(1);
	}
	
	listen(sfd, 100);

    epoll_fd = epoll_create(epoll_size);

	ev.events= EPOLLIN;
	ev.data.fd = sfd;
	
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &ev);


	socklen_t client_socklen;
	while(1)
	{
		nfds = epoll_wait(epoll_fd, events, 100, -1);
		for(n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == sfd)
			{
				cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_socklen);
				ev.events = EPOLLIN;
				ev.data.fd = cfd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cfd, &ev) == -1)
				{
					perror("epoll_ctl fail:");
					exit(1);
				}
			}
			else if (events[n].events == EPOLLIN)
			{
				recv(events[n].data.fd, buff, 100, 0);
				printf("%s", buff);
				close(events[n].data.fd);
				ev.events = EPOLLOUT;
				ev.data.fd = events[n].data.fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
			}
		}
	}
	close(epoll_fd);
	close(sfd);
}
