#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main(){
	//khai bao socket
	int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Khai bao dia chi cua server
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	//ket noi den server
	int res = connect(client, (struct sockaddr *)&addr, sizeof(addr));

	if(res == -1){
		printf("khong ket noi duoc den server!");
		return 1;

	}
	//gui tin nhan den server
	char msg[2048];
	printf("message send to server: \n");
	fgets(msg, sizeof(msg), stdin);
    msg[strlen(msg)] = 0;
	send(client, msg, strlen(msg), 0);
	//nhan tin nhan tu server
	char buf[2048];
	while(1){
		int len = recv(client, buf, sizeof(buf), 0);
	    buf[len] = 0;
	    printf("data received: %s\n", buf);
	}

	//dong socket
	close(client);
	return 0;
}    