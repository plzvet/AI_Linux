#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
	int sockfd;
	struct sockaddr_in sockaddr;
	socklen_t sock_size;
	int bytes_sent;

	char message1[]="Hello world ";
	char message2[]="Good morning ";
	char message3[]="I am a few good man";

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sockfd=socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		perror("socket() error!!");
		exit(1);
	}

	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family=AF_INET;
	sockaddr.sin_addr.s_addr=inet_addr(argv[1]);
	sockaddr.sin_port=htons(atoi(argv[2]));

	if(connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))==-1) {
		perror("connect() error!!");
		exit(1);
	}
	
	bytes_sent = send(sockfd, message1, strlen(message1), 0);
	printf("bytes_sent1 = %d\n", bytes_sent);
	bytes_sent = send(sockfd, message2, strlen(message2), 0);
	printf("bytes_sent2 = %d\n", bytes_sent);
	bytes_sent = send(sockfd, message3, strlen(message3), 0);
	printf("bytes_sent3 = %d\n", bytes_sent);

	close(sockfd);
	return 0;
}
