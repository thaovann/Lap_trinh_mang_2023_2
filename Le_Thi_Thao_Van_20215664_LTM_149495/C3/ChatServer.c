#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct
{
	int client_socket;
	char client_name[BUFFER_SIZE];
} ClientInfo;

int main()
{
	int server_socket, client_sockets[MAX_CLIENTS], max_clients = 0;
	ClientInfo client_info[MAX_CLIENTS];
	struct sockaddr_in server_addr, client_addr;
	char buffer[BUFFER_SIZE];

	// Create server socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		perror("Error in creating socket");
		exit(EXIT_FAILURE);
	}

	// Prepare server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(9000);

	// Bind server socket to address
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Error in binding");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_socket, 5) < 0)
	{
		perror("Error in listening");
		exit(EXIT_FAILURE);
	}

	fd_set readfds, tmpfds;
	int max_fd = server_socket + 1;

	// xoa tat ca cac socket ra khoi tap set
	FD_ZERO(&readfds);
	// Gan fd vao tap set
	FD_SET(server_socket, &readfds);

	while (1)
	{
		tmpfds = readfds;

		// Select for ready sockets
		if (select(max_fd, &tmpfds, NULL, NULL, NULL) < 0)
		{
			perror("Error in select");
			exit(EXIT_FAILURE);
		}

		// Check if server socket has incoming connection request
		if (FD_ISSET(server_socket, &tmpfds))
		{
			int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr);
			if (client_socket < 0)
			{
				perror("Error in accepting connection");
				exit(EXIT_FAILURE);
			}

			// Add client socket to list
			client_sockets[max_clients++] = client_socket;
			FD_SET(client_socket, &readfds);
			if (client_socket >= max_fd)
			{
				max_fd = client_socket + 1;
			}

			printf("New client connected\n");
		}

		// Check for data from clients
		for (int i = 0; i < max_clients; i++)
		{
			int client_socket = client_sockets[i];
			if (FD_ISSET(client_socket, &tmpfds))
			{
				memset(buffer, 0, BUFFER_SIZE);
				int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
				if (bytes_received <= 0)
				{
					// Client disconnected
					printf("Client disconnected\n");
					close(client_socket);
					FD_CLR(client_socket, &readfds);
					for (int j = i; j < max_clients - 1; j++)
					{
						client_sockets[j] = client_sockets[j + 1];
					}
					max_clients--;
					continue;
				}

				// Process received data
				if (strncmp(buffer, "client_id: ", 11) == 0)
				{
					char client_name[BUFFER_SIZE];
					sscanf(buffer + 11, "%s", client_name);
					// Store client information
					strcpy(client_info[i].client_name, client_name);
					printf("Client %d identified as '%s'\n", client_socket, client_name);
				}
				else
				{
					// Broadcast received message to other clients
					// Get current time
					time_t now;
					time(&now);
					struct tm *tm_info = localtime(&now);
					char time_str[BUFFER_SIZE];
					strftime(time_str, sizeof(time_str), "%Y/%m/%d %I:%M:%S%p", tm_info);

					// Broadcast received message to other clients with time
					for (int j = 0; j < max_clients; j++)
					{
						if (client_sockets[j] != client_socket)
						{
							char message[BUFFER_SIZE];
							snprintf(message, BUFFER_SIZE, "%s %s: %s", time_str, client_info[i].client_name, buffer);
							send(client_sockets[j], message, strlen(message), 0);
						}
					}
				}
			}
		}
	}

	close(server_socket);
	return 0;
}
