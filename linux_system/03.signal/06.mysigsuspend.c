#include <stdio.h> 
#include <signal.h> 
#include <unistd.h>
#include <stdlib.h>
#include "signalprint.h"

static void handler(int signo) { 
	printf ("[%d] signal is caughted\n", signo);
} 

int main() { 
	sigset_t set1,set2; 
	signal(SIGINT, handler); 
	signal(SIGRTMAX-1, handler);
	
	sigemptyset(&set1); 
	sigaddset(&set1,SIGINT); 
	sigaddset(&set1,SIGRTMAX-1); 
	
	sigfillset(&set2); 
	sigdelset(&set2, SIGINT);
	sigdelset(&set2, SIGRTMAX-1);
	
	sigprocmask(SIG_BLOCK,&set1,NULL); 
	print_sigset_t(&set1);
	print_sigset_t(&set2);
	
	for(int i=0; i<5; i++){
		printf("Critical Region 1 is running --- %d\n", i);
		sleep(1);
	}
#if 1
	printf("Waiting for a signal  ----------------- \n");
	sigsuspend(&set2);
#else
	sigprocmask(SIG_SETMASK, &set2, NULL);
	printf("Waiting for a signal  ----------------- \n");
	pause();
#endif
	for(int i=0; i<5; i++){
		printf("Critical Region 2 is running --- %d\n", i);
		sleep(1);
	}
	exit(0); 
}
