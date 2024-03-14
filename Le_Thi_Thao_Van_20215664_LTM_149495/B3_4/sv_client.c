#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(){
	char msg[2048];
	char IP_addr[100];
	int port;


	int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == -1){
		perror("Error creating socket");
		exit(1);
	}

	printf("IP address: \n");
	scanf("%s", IP_addr);
	IP_addr[strlen(IP_addr)] = 0;
	printf("Port: \n");
	scanf("%d", &port);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_addr);
    addr.sin_port = htons(port);


	// Tao ket noi
	int res = connect(client, (struct sockaddr *)&addr, sizeof(addr));
	if(res == -1){
		perror("Error connecting to server. \n");
		exit(1);
	}

	// nhan tin nhan tu server
	char buf[256];
    int len = recv(client, buf, sizeof(buf), 0);
    if (len == -1) {
        perror("Error receiving data from server");
    } else {
        buf[len] = '\0';
        printf("%s\n", buf);
    }

	printf("Start sending data: \n");
	msg[strcspn(msg, "\n")] = '\0'; // Remove trailing newline
	while(1){
		fgets(msg, sizeof(msg), stdin);
		msg[strcspn(msg, "\n")] = '\0'; 
		if(strncmp(msg, "end", 3) == 0) break;
		send(client, msg, strlen(msg), 0);
	}

	close(client);

	return 0;


}