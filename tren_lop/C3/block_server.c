#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

int main()
{
	// Tao socket
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener != -1)
		printf("Socket created: %d\n", listener);
	else
	{
		perror("socket() failed");
		return 1;
	}

	// khai bao cau truc dia chi server
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	// gan dia chi voi socket
	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
	{
		perror("bind() failed");
		return 1;
	}

	if (listen(listener, 5))
	{
		perror("listen() failed");
		return 1;
	}
	int clients[64];
	int numClients = 0;

	while (1)
	{

		printf("waiting for a new client ...\n");

		// chap nhan ket noi
		int client = accept(listener, NULL, NULL);
		if (client == -1)
		{
			printf("accept() failed.\n");
			exit(1);
		}

		clients[numClients] = client;
		numClients++;

		// Nhan du lieu tu client
		char buf[256];
		printf("Dang nhan du lieu tu cac client:");
		for (int i = 0; i < numClients; i++)
		{
			int ret = recv(client, buf, sizeof(buf), 0);
			if (ret <= 0)
			{
				continue;
			}

			buf[ret] = 0;
		    printf("%s nhan tu client: %d", buf, clients[i]);

			// gui du lieu sang client
		    send(client, buf, strlen(buf), 0);
		}

	
		// dong ket noi
		close(client);
		close(listener);

		return 0;
	}
}