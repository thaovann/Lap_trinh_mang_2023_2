#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>
int main()
{
	// khai bao socket
	int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Khai bao dia chi cua server
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	// ket noi den server
	int res = connect(client, (struct sockaddr *)&addr, sizeof(addr));

	if (res == -1)
	{
		printf("khong ket noi duoc den server!");
		return 1;
	}
	struct pollfd fds[2];
	fds[0].fd = STDIN_FILENO; // Mô tả của thiết bị nhập dữ liệu
	fds[0].events = POLLIN;
	fds[1].fd = client; // Mô tả của socket client
	fds[1].events = POLLIN;

	char buf[256];
	while (1)
	{
		int ret = poll;
		if (fds[0].revents | POLLIN)
		{
			fgets(buf, sizeof(buf), stdin);
			send(client, buf, strlen(buf), 0);
			if(ret <= 0){
				break;
			}

		}
	}
}