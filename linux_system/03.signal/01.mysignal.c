#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int repeat_int = 0;
int repeat_quit = 0;
int repeat_term = 0;
int repeat_tstp = 0;

void handler_int(int signo){
	fprintf(stderr, "Signal handler_int triggered %d times (signum=%d)\n", repeat_int++, signo);
	if(repeat_int >= 5)
		signal(SIGINT, SIG_DFL);
	if(signo == 15){
		sleep(3);
		exit(0);
	}
}

void handler_quit(int signo){
        fprintf(stderr, "Signal handler_quit triggered %d times (signum=%d)\n", repeat_quit++, signo);
        if(repeat_quit >= 5)
                signal(SIGINT, SIG_DFL);
        if(signo == 15){
                sleep(3);
                exit(0);
        }
}

void handler_term(int signo){
        fprintf(stderr, "Signal handler_term triggered %d times (signum=%d)\n", repeat_term++, signo);
        if(repeat_term >= 5)
                signal(SIGINT, SIG_DFL);
        if(signo == 15){
                sleep(3);
                exit(0);
        }
}

void handler_tstp(int signo){
        fprintf(stderr, "Signal handler_tstp triggered %d times (signum=%d)\n", repeat_tstp++, signo);
        if(repeat_tstp >= 5)
                signal(SIGINT, SIG_DFL);
        if(signo == 15){
                sleep(3);
                exit(0);
        }
}

int main(int argc, char *argv[]){
	signal(SIGINT, handler_int);
	signal(SIGQUIT, handler_quit);
	signal(SIGTERM, handler_term);
	signal(SIGTSTP, handler_tstp);
	while(1){
		printf("signal interrupt test --- %d\n", repeat_int);
		sleep(1);
	}
	return 0;
}


