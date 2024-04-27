#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 9000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

char *encode_string(const char *input_str)
{
	static char encoded_str[BUFFER_SIZE];
	memset(encoded_str, 0, sizeof(encoded_str));

	int i = 0;
	while (input_str[i] != '\0')
	{
		if (input_str[i] >= 'a' && input_str[i] <= 'z')
		{
			encoded_str[i] = ((input_str[i] - 'a' + 1) % 26) + 'a';
		}
		else if (input_str[i] >= 'A' && input_str[i] <= 'Z')
		{
			encoded_str[i] = ((input_str[i] - 'A' + 1) % 26) + 'A';
		}
		else if (input_str[i] >= '0' && input_str[i] <= '9')
		{
			encoded_str[i] = '9' - (input_str[i] - '0');
		}
		else
		{
			encoded_str[i] = input_str[i];
		}
		i++;
	}

	return encoded_str;
}

int main()
{
	int server_fd, client_fds[MAX_CLIENTS];
	struct sockaddr_in server_addr, client_addr;
	fd_set read_fds;
	char buffer[BUFFER_SIZE];

	// Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set socket options
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	// Bind socket
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_fd, 5) == -1)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	FD_ZERO(&read_fds);
	FD_SET(server_fd, &read_fds);

	int max_fd = server_fd;
	int num_clients = 0;

	while (1)
	{
		fd_set tmp_fds = read_fds;

		// Use select to determine sockets ready for reading
		if (select(max_fd + 1, &tmp_fds, NULL, NULL, NULL) == -1)
		{
			perror("select failed");
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i <= max_fd; i++)
		{
			if (FD_ISSET(i, &tmp_fds))
			{
				// Accept new connections
				if (i == server_fd)
				{
					socklen_t client_addr_len = sizeof(client_addr);
					int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
					if (client_fd == -1)
					{
						perror("accept failed");
						continue;
					}

					FD_SET(client_fd, &read_fds);
					if (client_fd > max_fd)
					{
						max_fd = client_fd;
					}

					client_fds[num_clients++] = client_fd;

					printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

					// Send greeting message with number of connected clients
					snprintf(buffer, sizeof(buffer), "Xin chào. Hiện có %d clients đang kết nối.\n", num_clients);
					send(client_fd, buffer, strlen(buffer), 0);
				}
				else
				{
					// Receive data from clients
					int bytes_received = recv(i, buffer, sizeof(buffer), 0);
					if (bytes_received <= 0)
					{
						close(i);
						FD_CLR(i, &read_fds);
						for (int j = 0; j < num_clients; j++)
						{
							if (client_fds[j] == i)
							{
								for (int k = j; k < num_clients - 1; k++)
								{
									client_fds[k] = client_fds[k + 1];
								}
								num_clients--;
								break;
							}
						}
						printf("Client disconnected\n");
					}
					else
					{
						buffer[bytes_received] = '\0';
						printf("Received from client: %s", buffer);

						if (strcmp(buffer, "exit\n") == 0)
						{
							// Send farewell message and close connection
							send(i, "Tạm biệt!\n", strlen("Tạm biệt!\n"), 0);
							close(i);
							FD_CLR(i, &read_fds);
							for (int j = 0; j < num_clients; j++)
							{
								if (client_fds[j] == i)
								{
									for (int k = j; k < num_clients - 1; k++)
									{
										client_fds[k] = client_fds[k + 1];
									}
									num_clients--;
									break;
								}
							}
							printf("Client disconnected\n");
						}
						else
						{
							// Encode received string and send back to client
							char *encoded_data = encode_string(buffer);
							send(i, encoded_data, strlen(encoded_data), 0);
						}
					}
				}
			}
		}
	}

	return 0;
}
