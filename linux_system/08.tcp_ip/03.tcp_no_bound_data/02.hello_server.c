#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
	int server_sfd, client_sfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t sock_size;
	char message[128];					//should be big enough to place the entire tcp_rx_buf
	int bytes_recv=0, idx=0;
	char t_rxbuf[20];					//should be enough to place the receive() size
	int yes = 1;
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	server_sfd=socket(PF_INET, SOCK_STREAM, 0);
	if(server_sfd == -1)	{
		perror("socket() error!!");
		exit(1);
	}

	if(setsockopt(server_sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		perror("setsockopt() error!!");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(server_sfd, (struct sockaddr*) &server_addr, sizeof(server_addr))==-1 ) { 
		perror("bind() error!!");
		exit(1);
	}
	
	if(listen(server_sfd, 5)==-1) {
		perror("listen() error!!");
		exit(1);
	}
	
	sock_size=sizeof(client_addr);  
	client_sfd=accept(server_sfd, (struct sockaddr*)&client_addr,&sock_size);
	if(client_sfd==-1) {
		perror("accept() error!!");
		exit(1);
	}

	bzero(&message, sizeof(message));
#if 0
	bytes_recv=recv(client_sfd, message, sizeof(message), 0);
#else
	while( bytes_recv=recv(client_sfd, &message[idx], 8, 0) ) {
		printf("Receiving index %d(%d)\n", idx, bytes_recv);
		if(bytes_recv == 0) {
			break;
		} else {
			memset(t_rxbuf, 0, sizeof(t_rxbuf));
			strncpy(t_rxbuf, &message[idx], bytes_recv);			
			printf("recv ok(%d:%s)\n", idx, t_rxbuf);
			idx += bytes_recv;
		}
		bytes_recv += bytes_recv;
	}
#endif
	
	printf("Message from %s: %s\n", inet_ntoa(client_addr.sin_addr), message);  
	close(client_sfd);
	close(server_sfd);
	return 0;
}
