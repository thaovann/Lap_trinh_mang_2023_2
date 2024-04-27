#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUF_SIZE 1024

int main()
{
	char receiver_ip[INET_ADDRSTRLEN];
	int receiver_port;

	printf("Enter receiver IP address: ");
	if (fgets(receiver_ip, INET_ADDRSTRLEN, stdin) == NULL)
	{
		fprintf(stderr, "Error reading receiver IP address.\n");
		return 1;
	}
	receiver_ip[strcspn(receiver_ip, "\n")] = '\0'; // Remove newline character

	printf("Enter receiver port: ");
	if (scanf("%d", &receiver_port) != 1)
	{
		fprintf(stderr, "Error reading receiver port.\n");
		return 1;
	}

	// Create a UDP socket
	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	// Set up address structure for the receiver
	struct sockaddr_in receiver_addr;
	memset(&receiver_addr, 0, sizeof(receiver_addr));
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = inet_addr(receiver_ip);
	receiver_addr.sin_port = htons(receiver_port);

	fd_set readfds;
	char buf[BUF_SIZE];

	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
		FD_SET(sockfd, &readfds);

		// Wait for activity on either stdin or the socket
		if (select(sockfd + 1, &readfds, NULL, NULL, NULL) < 0)
		{
			perror("select");
			break;
		}

		// If there is activity on stdin, send message to receiver
		if (FD_ISSET(STDIN_FILENO, &readfds))
		{
			fgets(buf, BUF_SIZE, stdin);
			sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
		}

		// If there is activity on the socket, receive and print message
		if (FD_ISSET(sockfd, &readfds))
		{
			struct sockaddr_in sender_addr;
			socklen_t sender_len = sizeof(sender_addr);
			ssize_t recv_len = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&sender_addr, &sender_len);
			if (recv_len < 0)
			{
				perror("recvfrom");
				break;
			}
			buf[recv_len] = '\0';
			printf("Received message: %s", buf);
		}
	}

	close(sockfd);
	return 0;
}
