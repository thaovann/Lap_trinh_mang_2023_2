#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1) {
        perror("Socket creation failed");
        return 1;
    }

	char input[100];
    char IP_addr[100];
    int port;
	scanf("%s", input);
    scanf("%s", IP_addr);
	scanf("%d", &port);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_addr);
    addr.sin_port = htons(port);

    // Connect to server
    int res = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1) {
        perror("Error connecting to server");
        close(client);
        return 1;
    }
    
	char buf[256];
    int len = recv(client, buf, sizeof(buf), 0);
    if (len == -1) {
        perror("Error receiving data from server");
    } else {
        buf[len] = '\0';
        printf("%s\n", buf);
    }

    len = recv(client, buf, sizeof(buf), 0);
      if (len == -1) {
        perror("Error receiving data from server");
    } else {
        buf[len] = '\0';
        printf("%s\n", buf);
    }

    // Send message to server
    char msg[2000];
    while (1) {
        fgets(msg, sizeof(msg), stdin);
        msg[strlen(msg)] = 0;
        send(client, msg, strlen(msg), 0);
        if (strncmp(msg, "exit", 4) == 0)
            break;
    }

    // Close socket
    close(client);

    return 0;
}
