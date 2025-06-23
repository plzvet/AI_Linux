#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
	in_addr_t ipaddr;
	struct sockaddr_in sock_addr; 
	struct in_addr sip_addr;
	unsigned char *addr;
	
	ipaddr=inet_network(argv[1]); 	// host_addr=inet_network("192.168.60.100");
	printf("inet_network() = %#x\n", ipaddr);
	addr = (unsigned char *)&ipaddr;
	printf("MEM-ADDR = %p ", addr);
	printf(" --> %#x ", *addr);
	printf("\t%#x %#x %#x %#x\n", *addr, *addr++, *addr++, *addr++);
	
	sock_addr.sin_addr.s_addr=inet_addr(argv[1]);
	ipaddr=sock_addr.sin_addr.s_addr;
	printf("inet_addr() = %#x\n", ipaddr);
	addr = (unsigned char *)&ipaddr;
	printf("MEM-ADDR = %p ", addr);
	printf(" --> %#x ", *addr);
	printf("\t%#x %#x %#x %#x\n", *addr, *addr++, *addr++, *addr++);
	
	inet_aton(argv[1], &sip_addr);
	printf("inet_aton() = %#x\n", sip_addr.s_addr);
	ipaddr=sip_addr.s_addr;
	addr = (unsigned char *)&ipaddr;
	printf("MEM-ADDR = %p ", addr);
	printf(" --> %#x ", *addr);
	printf("\t%#x %#x %#x %#x\n", *addr, *addr++, *addr++, *addr++);
	
	
	inet_pton(AF_INET, argv[1], &(sock_addr.sin_addr));
	printf("inet_pton() = %#x \n", sock_addr.sin_addr.s_addr);
	
	addr=inet_ntoa(sock_addr.sin_addr);
	printf("inet_ntoa() = %s\n", addr);
	return 0;
}