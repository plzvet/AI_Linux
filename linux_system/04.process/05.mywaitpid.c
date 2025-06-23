#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(void) {
	int status=0;
	pid_t pid, exit_pid;
	time_t startTimec, startTimep;
	
	switch(pid = fork()){
		case -1:		// fork error
			perror("fork"); 
			exit(1);
		case 0:						//Child 
			for (startTimec = time(NULL); time(NULL) < startTimec + 8;){
				for(int i=0; i<100000000; i++);
				write(1, "+", 1);
			}
			printf("\n");
			system("ps -lf");
			exit(0);
		default:					//Parent
			for (startTimep = time(NULL); time(NULL) < startTimep + 10;){
				for(int i=0; i<100000000; i++);
				write(1, "*", 1);
				exit_pid = waitpid(-1, &status, WNOHANG);
			}
			printf("\n");
			system("ps -lf");
	}
	return 0;
}





