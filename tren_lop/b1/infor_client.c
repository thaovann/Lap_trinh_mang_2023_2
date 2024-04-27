#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
int main(){
	struct FileData {
        char name[256];
        long size;
    };
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

	// Lay ten thu muc hien tai
	char cwd[1024];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

	//gui tin nhan den server
	char msg[2048];
	printf("message send to server: %s \n", cwd);
	send(client, cwd, strlen(cwd), 0);

	//nhan tin nhan tu server
	char buf[2048];
	int len = recv(client, buf, sizeof(buf), 0);
	buf[len] = 0;
	printf("data received: %s\n", buf);

	// Lấy danh sách các tập tin và kích thước của chúng
    struct dirent *dir_entry;
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Unable to open directory");
        return 1;
    }

    struct FileData file_data[100]; // Giả sử không quá 100 tập tin trong thư mục
    int file_count = 0;

    while ((dir_entry = readdir(dir)) != NULL) {
        if (dir_entry->d_type == DT_REG) { // Kiểm tra xem có phải là tập tin không
            strcpy(file_data[file_count].name, dir_entry->d_name);

            // Lấy thông tin chi tiết về tập tin để lấy kích thước
            struct stat file_stat;
            if (stat(dir_entry->d_name, &file_stat) == 0) {
                file_data[file_count].size = file_stat.st_size;
            } else {
                file_data[file_count].size = -1; // Gán -1 nếu không thể lấy kích thước
            }

            file_count++;
        }
    }

    closedir(dir);

    // Gửi thông điệp chứa danh sách các tập tin và kích thước của chúng đến server
    send(client, file_data, sizeof(file_data), 0);

	//dong socket
	close(client);
	return 0;
}    