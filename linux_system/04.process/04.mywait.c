#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void) {
	int status;
	pid_t pid;
	
	if((pid=fork()) == 0){
		system("ps -l");
		printf("Child (%d) waiting for either a signal or timeout\n", getpid());
#if 0
		pause();
#else
		sleep(3);
#endif
		exit(10);
	} else {
		printf("Parent (%d) waiting for termination of child\n", getpid());
		
		pid = wait(&status);
		
		if (pid == -1)
			perror ("wait");
		printf("PID of Child terminated = %d\n", pid);
		if (WIFEXITED(status))
			printf ("Normal termination with exit status=%d\n", WEXITSTATUS(status));
		if (WIFSIGNALED(status))
			printf ("terminated by signal=%d%s\n", WTERMSIG(status), WCOREDUMP(status) ? \
																	" (dumped core)" : "");
		system("ps -l");
	}
	return 0;
}


