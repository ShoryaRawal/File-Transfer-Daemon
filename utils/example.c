#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<netinet/in.h>

#define BUFFER_SIZE 1024

void daemonize(){
	pid_t pid;
	pid = fork();
	if(pid < 0) exit(EXIT_FAILURE);
	if (pid > 0) exit(EXIT_SUCCESS);

	if(setsid() < 0) exit(EXIT_FAILURE);
	if(chdir("/") < 0) exit(EXIT_FAILURE);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	dup(0);
	dup(0);

	umask(0);

	FILE * pid_file = fopen("/var/run/mydaemon.pid", "w");
	if(pid_file != NULL){
		fprintf(pid_file, "%d\n", getpid());
		fclose(pid_file);
	}
}

void signal_handler(int signum){
	printf("Received signal %d, exiting...\n", signum);
	exit(EXIT_SUCCESS);
}



int main(){
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	char path[32];
	char buffer[BUFFER_SIZE];

	FILE * file;

	getcwd(path, 32);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8000);

	daemonize();
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	bind(server_socket, (struct sockaddr *) & server_address, sizeof(server_address));
	listen(server_socket, 5);
	while(1){
		int client_socket = accept(server_socket, NULL, NULL);
		int bytes_read;
		//char buffer[64];
		//int bytes_read = read(client_socket, buffer, sizeof(buffer));
		//strcat(path, "/example.c");
		write(client_socket, "Init", 5);
		file = fopen("/home/fed/c/daemon/example.c", "rb");
		if(file == NULL) write(client_socket, "Error, Opening File", 19);
		else{
			while((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0){
				ssize_t bytes_written = write(client_socket, buffer, bytes_read);
				if(bytes_written < 0) break;
				else if(bytes_written < bytes_read){
					int remaining = bytes_read - bytes_written;
					while(remaining > 0){
						bytes_written = write(client_socket, buffer + bytes_read - remaining, remaining);
						if(bytes_written < 0) break;
						remaining -= bytes_written;
					}
				}
			}
			/*while(buffer > 0){
				fread(buffer, BUFFER_SIZE, 1, file);
				if(buffer > 0) write(client_socket, buffer, BUFFER_SIZE);
				else{
					close(client_socket);
					break;
				}
			}*/
		}
		fclose(file);
		sleep(60);
	}
	close(server_socket);
	return 0;
}
