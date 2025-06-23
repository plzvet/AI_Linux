#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>   
#include <sys/wait.h>

/* 
	Original example code has Zombie process left behind.
	In order to get rid of those Zombie process, it is mandatory, in this example, 
	to hand signal SIGCHLD which would be received when a child process is dead.
*/

void handler(int signo) {
	int ret;
	int status;
	
	printf("Signal Handler starts -----\n");
	ret = waitpid(-1, &status, WNOHANG);
	if(ret == 0) {
		printf("PID %d child process is dead\n", ret);
		// break;
	}
	if(ret == -1 && errno == ECHILD) {
		printf("No Child --\n");
		// break;
	}
	if(ret == -1) {
		perror("child waitpid");
		abort();
	}
	printf("PID %d child process is dead\n", ret);
	printf("Signal Handler ends -----\n");
}

int main(void) {
	int server_sfd, client_sfd, bytes_recv;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sock_size;
	int yes = 1;
	char tx_buf[128], rx_buf[128];
	int i;
	
	signal(SIGCHLD, handler);

	if((server_sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() error");
		exit(1);
	}

	if(setsockopt(server_sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt() error");
		exit(1);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(10000); //server port number setting
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(server_addr.sin_zero), '\0', 8);

	//server ip & port number setting
	if(bind(server_sfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind() error");
		exit(1);
	}

	//client backlog setting
	if(listen(server_sfd, 5) == -1) {
		perror("listen() error");
		exit(1);
	}

	while(1) {
		
		sock_size = sizeof(struct sockaddr_in);

		//wait for client request
		if((client_sfd = accept(server_sfd, (struct sockaddr *)&client_addr, &sock_size)) == -1) {
			perror("accept() error");
			continue;
		}

		printf("server : got connection from %s \n", inet_ntoa(client_addr.sin_addr));

		if(!fork())	{
			close(server_sfd);
			for(i=1; ; i++){
				memset(tx_buf, 0, sizeof(tx_buf));
				memset(rx_buf, 0, sizeof(rx_buf));
				//wait for rx data from client	
				if((bytes_recv = recv(client_sfd, rx_buf, sizeof(rx_buf), 0)) == -1) {
					perror("recv");
					exit(1);
				}
				if(bytes_recv == 0) 
					break;	
				printf("Server Rx(%d) : %s", getpid(), rx_buf);
				sprintf(tx_buf, "Hi_%d, client(from %s)~~\n", i, inet_ntoa(server_addr.sin_addr));
				//send data to client
				if(send(client_sfd, tx_buf, strlen(tx_buf)+1, 0) == -1) 
					perror("send");	
			}
			printf("Server(%d): Client Connection Socket Closed!!\n", getpid());
			//close client socket connection		
			close(client_sfd);
			exit(0);
		}
	}
	return 0;
}

