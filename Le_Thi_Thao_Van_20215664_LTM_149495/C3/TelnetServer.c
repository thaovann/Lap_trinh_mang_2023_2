#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct
{
	int client_socket;
	int authenticated;
} ClientInfo;

int authenticate(const char *username, const char *password)
{
	FILE *db;
	db = fopen("users.txt", "r");
	if (db == NULL)
	{
		printf("Error opening the file.\n");
		return 1;
	}

	char line[BUFFER_SIZE];
	while (fgets(line, sizeof(line), db))
	{
		char db_username[BUFFER_SIZE];
		char db_password[BUFFER_SIZE];
		sscanf(line, "%s %s", db_username, db_password);
		if (strcmp(username, db_username) == 0 && strcmp(password, db_password) == 0)
		{
			fclose(db);
			return 1; 
		}
	}

	fclose(db);
	return 0; 
}

int main()
{
	int server_socket, client_sockets[MAX_CLIENTS], max_clients = 0;
	ClientInfo client_info[MAX_CLIENTS];
	struct sockaddr_in server_addr, client_addr;
	char buffer[BUFFER_SIZE];

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
	if (listen(server_socket, 5) < 0)
	{
		perror("Error in listening");
		exit(EXIT_FAILURE);
	}

	fd_set readfds, tmpfds;
	int max_fd = server_socket + 1;

	FD_ZERO(&readfds);
	FD_SET(server_socket, &readfds);

	while (1)
	{
		tmpfds = readfds;
		if (select(max_fd, &tmpfds, NULL, NULL, NULL) < 0)
		{
			perror("Error in select");
			exit(EXIT_FAILURE);
		}
		if (FD_ISSET(server_socket, &tmpfds))
		{
			int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr);
			if (client_socket < 0)
			{
				perror("Error in accepting connection");
				exit(EXIT_FAILURE);
			}
			client_sockets[max_clients++] = client_socket;
			send(client_socket,"user and pass: \n", strlen("user and pass: \n"), 0);
			FD_SET(client_socket, &readfds);
			if (client_socket >= max_fd)
			{
				max_fd = client_socket + 1;
			}

			printf("New client connected\n");
			
		}
		for (int i = 0; i < max_clients; i++)
		{
			int client_socket = client_sockets[i];
			if (FD_ISSET(client_socket, &tmpfds))
			{
				memset(buffer, 0, BUFFER_SIZE);
				int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
				if (bytes_received <= 0)
				{
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
				if (client_info[i].authenticated)
				{
					FILE *cmd_output = popen(buffer, "r");
					if (cmd_output == NULL)
					{
						perror("Error executing command");
						send(client_socket, "Error executing command\n", strlen("Error executing command\n"), 0);
					}
					else
					{
						char output_buffer[BUFFER_SIZE];
						while (fgets(output_buffer, sizeof(output_buffer), cmd_output) != NULL)
							send(client_socket, output_buffer, strlen(output_buffer), 0);
						pclose(cmd_output);
					}
				}
				else
				{
					char username[BUFFER_SIZE];
					char password[BUFFER_SIZE];
					sscanf(buffer, "%s %s", username, password);
					if (authenticate(username, password))
					{
						client_info[i].authenticated = 1;
						send(client_socket, "Authentication successful\n", strlen("Authentication successful\n"), 0);
					}
					else
					{
						send(client_socket, "Authentication failed\n", strlen("Authentication failed\n"), 0);
					}
				}
			}
		}
	}

	close(server_socket);
	return 0;
}
