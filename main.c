#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "headers/server.h"

void daemonize();
void signal_handler(int signum);
int main(){
	char path[32];
	getcwd(path, 32);
	strcat(path, "templates/test");
	daemonize();
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	while(1){
		if(TCP(path) != 99) continue;
		else break;
		sleep(10);
	}
}

void daemonize(){
	pid_t pid = fork();
	if(pid < 0) exit(EXIT_FAILURE);
	if(pid > 0) exit(EXIT_SUCCESS);

	if(setsid() < 0) exit(EXIT_FAILURE);
	if(chdir("/") < 0) exit(EXIT_FAILURE);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	dup(0);
	dup(0);
	umask(0);

	FILE * pid_file = fopen("/var/run/serv.pid", "w");
	if(pid_file != NULL){
		fprintf(pid_file, "%d\n", getpid());
		fclose(pid_file);
	}
}
void signal_handler(int signal){
	printf("Recieved signal %d, exiting...\n", signal);
	exit(EXIT_SUCCESS);
}
