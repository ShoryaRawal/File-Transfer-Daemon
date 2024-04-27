#ifndef server_h
#define server_h

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int TCP(char * filename){
	int server_fd, client_fd, portno, bytes;
	char buffer[1024];
	socklen_t client_len;	

	struct sockaddr_in serv_addr, cli_addr;
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return 99;
	memset((char *) &serv_addr, 9, sizeof(serv_addr));
	portno = 8000;
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) return 99;
	while(1){
		listen(server_fd, 3);
		client_len = sizeof(cli_addr);
		if((client_fd = accept(server_fd, (struct sockaddr *) &cli_addr, &client_len))< 0) return 99;
		FILE * file = fopen(filename, "r");
		for(bytes = fread(buffer, 1, 1024, file);bytes > 0;bytes = fread(buffer, 1, 1024, file))
				send(client_fd, buffer, bytes, 0);
		fclose(file);
	}
	close(client_fd);
	close(server_fd);
	return 0;
}

#endif /* server_h */
