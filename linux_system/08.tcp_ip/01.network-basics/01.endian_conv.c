#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	unsigned char *addr;
	unsigned int host_addr;
	unsigned int net_addr;
	
	struct sockaddr_in sock_addr;

	host_addr=0xc0a83865;
	printf("LIEELE ENDIAN = %#x\n", host_addr);
	addr = (unsigned char *)&host_addr;
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	
	sock_addr.sin_addr.s_addr=host_addr;
	printf("LITTLE ENDIAN IP_ADDR = %s\n\n", inet_ntoa(sock_addr.sin_addr));
	
	net_addr=htonl(host_addr);
	printf("BIG ENDIAN = %#x\n", net_addr);
	addr = (unsigned char *)&net_addr;
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	printf("MEM-ADDR = %p \t\t%#x\n", addr, *addr++);
	
	sock_addr.sin_addr.s_addr=net_addr;	
	printf("BIG ENDIAN IP_ADDR = %s\n", inet_ntoa(sock_addr.sin_addr));
	return 0;
}

