#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 9000
#define MAX_CONNECTIONS 5

void handle_client(int client_socket)
{
	char buffer[256];
	char response[256];

	int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
	if (bytes_received < 0)
	{
		perror("Error receiving data from client");
		close(client_socket);
		return;
	}

	buffer[bytes_received] = '\0';

	if (strncmp(buffer, "GET_TIME", 8) == 0)
	{

		char *format_start = strstr(buffer, "[");
		if (format_start != NULL)
		{
			format_start++; 
			char *format_end = strchr(format_start, ']');
			if (format_end != NULL)
			{
				*format_end = '\0'; 
				char *format = format_start;

			
				time_t now;
				struct tm *local_time;
				time(&now);
				local_time = localtime(&now);

				if (strcmp(format, "dd/mm/yyyy") == 0)
				{
					strftime(response, sizeof(response), "%d/%m/%Y", local_time);
				}
				else if (strcmp(format, "dd/mm/yy") == 0)
				{
					strftime(response, sizeof(response), "%d/%m/%y", local_time);
				}
				else if (strcmp(format, "mm/dd/yyyy") == 0)
				{
					strftime(response, sizeof(response), "%m/%d/%Y", local_time);
				}
				else if (strcmp(format, "mm/dd/yy") == 0)
				{
					strftime(response, sizeof(response), "%m/%d/%y", local_time);
				}
				else
				{
					strcpy(response, "Invalid format");
				}

				send(client_socket, response, strlen(response), 0);
			}
			else
			{
				
				char *error_message = "Invalid format";
				send(client_socket, error_message, strlen(error_message), 0);
			}
		}
		else
		{
			char *error_message = "Invalid format";
			send(client_socket, error_message, strlen(error_message), 0);
		}
	}
	else
	{
		char *error_message = "Invalid request";
		send(client_socket, error_message, strlen(error_message), 0);
	}

	
	close(client_socket);
}

int main()
{
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);

	// Create socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Set server address structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// Bind socket to address and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Binding failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_socket, MAX_CONNECTIONS) < 0)
	{
		perror("Listening failed");
		exit(EXIT_FAILURE);
	}

	printf("Server is listening on port %d\n", PORT);

	// Main loop to accept incoming connections
	while (1)
	{
		// Accept connection
		if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) < 0)
		{
			perror("Acceptance failed");
			exit(EXIT_FAILURE);
		}

		printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		// Handle client request
		handle_client(client_socket);
	}

	// Close server socket (this part of code may not be reached)
	close(server_socket);

	return 0;
}
