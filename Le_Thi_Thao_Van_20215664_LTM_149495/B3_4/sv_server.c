#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(){
	int port;
	char file_name[256];
	
	//Khoi tao socket
	
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener == -1){
		perror("Socket creation failed. \n");
		exit(1);
	}

	// khoi tao dia chi

	printf("Please, input port: \n");
	scanf("%d", &port);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);


	//gan dia chi IP voi socket
	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr))){
		perror("Assigning IP address and port failed \n");
		exit(1);
	}

	
	if(listen(listener, 5) == -1){
		perror("listen() failed. \n");
		exit(1);
	}

	printf("Waiting for a new client ... \n");

	//Accept connection
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);
	if(client == -1){
		perror("Connection failed. \n");
		exit(1);
	}

	char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("Connected with %s\n", client_ip);


	char msg[2048] = "Hi, client!";
	send(client, msg, sizeof(msg), 0);

	printf("You want to save data to: \n");
	
	char f_name[256];
	scanf("%s", f_name);
	FILE *f = fopen(f_name,"wb");

	// starting receive and write data to file
	char buf[1024];
    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;

		// Get current time
        time_t current_time;
        struct tm *tm_info;
        time(&current_time);
        tm_info = localtime(&current_time);

        // Format time string
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        if (ret < sizeof(buf))
            buf[ret] = 0;
        
		fprintf(f, "%s %s %s\n", client_ip, time_str, buf);
        puts(buf);
    }


	fclose(f);
	close(client);
	close(listener);

	return 0;
	








}