#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>


int main(){
	//Tao socket
	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener != -1)
	    printf("Socket created: %d\n", listener);
	else
	{
		perror("socket() failed");
		return 1;
	}


	//khai bao cau truc dia chi server
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	//gan dia chi voi socket
	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr))){
		perror("bind() failed");
        return 1;
	}


	if(listen(listener, 5))
	{
		perror("listen() failed");
        return 1;
	}

	printf("waiting for a new client ...\n");

	//chap nhan ket noi
	int client = accept(listener, NULL, NULL);
	if(client == -1){
		printf("accept() failed.\n");
		exit(1);
	}

	printf("Client moi ket noi: %d - IP: %s - Port: %d\n", 
        client, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


		
	//Nhan du lieu tu client
	char buf[256];
	int ret = recv(client, buf, sizeof(buf), 0);
	if (ret <= 0){
		printf("recv() failed. \n");
		exit(1);
	}

	//them ky tu ket thuc xau va in ra man hinh
	if (ret < sizeof(buf))
	buf[ret] = 0;
    puts(buf);

	//gui du lieu sang client
	send(client, buf, strlen(buf), 0);

	//dong ket noi
	close(client);
	close(listener);

	return 0;


}