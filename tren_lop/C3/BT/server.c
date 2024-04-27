#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define MAX_FDS 256
int main()
{
	// Tao socket cho ket noi
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener == -1)
	{
		perror("socket() failed");
		return 1;
	}

	// Khai bao dia chi server
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	// Gan socket voi cau truc dia chi
	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
	{
		perror("bind() failed");
		return 1;
	}

	// Chuyen socket sang trang thai cho ket noi
	if (listen(listener, 5))
	{
		perror("listen() failed");
		return 1;
	}
	struct pollfd fds[MAX_FDS];
	int nfds = 0;
	fds[0].fd = listener;
	fds[0].events = POLLIN;
	nfds++;

	char buf[256];
	while (1)
	{
		int ret = poll(fds, nfds, -1);
		if (ret == -1)
			break;

		for (int i = 0; i < nfds; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd = listener)
				{
					// cos ker noi moi
					int client = accept(listener, NULL, NULL);
					if (client >= MAX_FDS)
					{
						close(client);
					}
					else
					{
						fds[nfds].fd = client;
						fds[nfds].events = POLLIN;
						nfds++;
					}
				}
				else
				{
					int client = fds[i].fd;
					ret = recv(client, buf, sizeof(buf), 0);
					if (ret <= 0)
					{
					}
					else
					{
						buf[ret] = 0;
						printf("recieved from %d\n: %s", client, buf);

						for (int j = 0; j < nfds; j++){
							if(fds[j].fd != listener && fds[j].fd !=client  ){
								send(client, buf, sizeof(buf), 0);
							}
						}
					}

				}
			}
		}
	}

	return 1;
}