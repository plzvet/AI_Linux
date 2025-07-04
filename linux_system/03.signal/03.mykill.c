#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	
#if 1
	if(argc < 3){
		fprintf(stderr, "Usage: %s <pid_#> <sig_#>\n", argv[0]);
		exit(1);
	}
	if(kill(atoi(argv[1]), atoi(argv[2])) == -1){
		if(errno == EINVAL){
			printf("Invalid signal\n");
		}else if(errno == EPERM){
			printf("No Permission\n");
		}else if(errno == ESRCH){
			printf("No such process\n");
		}
	}
#else
	if(argc < 2){
		fprintf(stderr, "Usage: %s <pid_#>\n", argv[0]);
		exit(1);
	}
	for (int i=34; i<64; i++){
		kill(atoi(argv[1]), i);
		sleep(3);
	}
#endif
	return 0;
}


