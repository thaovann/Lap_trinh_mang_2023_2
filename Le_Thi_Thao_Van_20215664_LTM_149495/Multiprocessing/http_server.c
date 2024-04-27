#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define NUM_PROCESSES 4 // Số lượng tiến trình con sẽ được tạo sẵn

void handle_client(int client_socket)
{
	char buf[256];
	int ret = recv(client_socket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	puts(buf);

	// Trả lại kết quả cho client
	char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</h1></body></html>";
	send(client_socket, msg, strlen(msg), 0);

	// Đóng kết nối
	close(client_socket);
}

int main()
{
	int listener, client;
	struct sockaddr_in server_addr;

	// Tạo socket
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Thiết lập địa chỉ và cổng của server
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// Bind socket với địa chỉ và cổng
	if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("Binding failed");
		exit(EXIT_FAILURE);
	}

	// Lắng nghe kết nối từ client
	if (listen(listener, 10) == -1)
	{
		perror("Listening failed");
		exit(EXIT_FAILURE);
	}

	// Tạo sẵn các tiến trình con
	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		if (fork() == 0)
		{
			// Tiến trình con sẽ xử lý client
			while (1)
			{
				// Chờ kết nối mới
				client = accept(listener, NULL, NULL);
				printf("New client connected in process %d: %d\n", getpid(), client);

				// Xử lý client
				handle_client(client);
			}
		}
	}

	// Chờ cho tất cả các tiến trình con kết thúc
	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		wait(NULL);
	}

	// Đóng socket
	close(listener);

	return 0;
}
