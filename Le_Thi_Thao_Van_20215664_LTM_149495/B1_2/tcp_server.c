#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define MAX_LINE 2048

int main() {
    // Tạo socket
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        perror("Tao socket that bai.\n");
        return 1;
    }

    int port;
    printf("Nhap vao cong: \n");
    scanf("%d", &port);

    // Khai báo cấu trúc địa chỉ
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // Gan dia chi IP voi socket
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind() failed.\n");
        exit (1);
    }

    if (listen(listener, 5) == -1) {
        perror("listen() failed.\n");
        exit (1);
    }

    printf("waiting for a new client ...\n");

    // Chap nhan ket noi
    int client = accept(listener, NULL, NULL);
    if (client == -1) {
        perror("accept() failed.\n");
        exit (1);
    }

    // Open file------------------------------------------------------------------------------------
    FILE *file = fopen("text.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit (1);
    }

    // Read file content
    char file_buffer[MAX_LINE];
    size_t file_size = fread(file_buffer, 1, MAX_LINE, file);
    fclose(file);

    // Send file content to client
    ssize_t bytes_sent = send(client, file_buffer, file_size, 0);
    if (bytes_sent < 0) {
        perror("Error sending data to client");
        exit (1);
    }

	//nhan du lieu tu client ------------------------------------------------------------------------------------
	printf("You want to save data from client to: \n");
	
	char f_name[1024];
	scanf("%s", f_name);
	FILE *f = fopen(f_name,"wb");

    char msg[2048];
    strcpy(msg, "You can start sending: \n");
    send(client, msg, sizeof(msg), 0);


    char buf[1024];
    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        
        if (ret < sizeof(buf))
            buf[ret] = 0;
        
		fwrite(buf, 1, ret, f);
        puts(buf);
    }
	
	fclose(f);
    // Close connections
    close(client);
    close(listener);

    return 0;
}
