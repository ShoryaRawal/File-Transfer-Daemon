#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8000
#define BUFFER_SIZE 1024

int main(){
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	FILE * file;

	if(client_socket < 0){
		perror("Error creating a socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
	server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	if(connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
		perror("Error conecting to the server");
		close(client_socket);
		exit(EXIT_FAILURE);
	}

	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	file = fopen("received_file", "wb");
	if(file == NULL) exit(EXIT_FAILURE);

	memset(buffer, 0, BUFFER_SIZE);

	while((bytes_read = read(client_socket, buffer, BUFFER_SIZE)) > 0){
		fwrite(buffer, 1, bytes_read, file);
		memset(buffer, 0, BUFFER_SIZE);
	}
	/*strcpy(buffer, "Hello, Server");
	write(client_socket, buffer, strlen(buffer));

	bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
	if(bytes_read > 0){
		buffer[bytes_read] = '\0';
		printf("Recieved from server: :%s\n", buffer);
	}
	*/
	fclose(file);
	close(client_socket);
	return 0;
}
