#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "signalprint.h"

void handler(int signo){
	printf ("[%d] signal is caughted\n", signo);
}

int main(void) {
	sigset_t set1, set2, set3, set4, oldset;
	
	signal(SIGQUIT, handler);  // ctrl + \
	sleep(2);
	sigemptyset(&set1);	
	sigaddset(&set1, 2);
	printf("signals in set1 are blocked\n");
	printf("set1 : ");
	print_sigset_t(&set1);	
	sigprocmask(SIG_BLOCK, &set1, &oldset);
	printf("oldset: ");
	print_sigset_t(&oldset);
	
	sleep(5);
	sigemptyset(&set2);							
	sigaddset(&set2, SIGQUIT);  
	printf("\nSignals in set2 are blocked\n");	
	printf("set2 : ");
	print_sigset_t(&set2);	
	sigprocmask(SIG_BLOCK, &set2, &oldset);
	printf("oldset: ");
	print_sigset_t(&oldset);
	
	sleep(5);
	sigemptyset(&set4);							
	sigaddset(&set4, SIGRTMIN);  
	printf("\nSignals in set4 are now blocked\n");	
	printf("set4 : ");
	print_sigset_t(&set4);	
	sigprocmask(SIG_BLOCK, &set4, &oldset);
	printf("oldset: ");
	print_sigset_t(&oldset);

	sleep(5);
	sigfillset(&set3);							
	printf("\nThe set of blocked signal is replaced by set3\n");
	printf("set3 : ");
	print_sigset_t(&set3);
	sigprocmask(SIG_SETMASK, &set3,  &oldset);	
	printf("oldset : ");
	print_sigset_t(&oldset);
	
	sleep(5);
	printf("\nSignals in oldset are now unblocked\n");
	sigprocmask(SIG_UNBLOCK, &oldset, NULL);			
		
	for(int i=0; ; i++){    
		printf("Hello Signal --- %d\n", i);    
		sleep(1);  
	}
	return 0;
}

