#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int signo){
        printf("Received Signal no - %d\n", signo);
    if(signo == 34)
        fprintf(stderr, "Hi Linux\n");
    else if(signo == 35)
        fprintf(stderr, "Good bye Linux\n");
}

int main(int argc, char *argv[]){


    if (signal(34, handler) == SIG_ERR) {
        //fprintf(stderr, "Cannot handle signal=34\n");
        return 1;
    }
    if (signal(35, handler) == SIG_ERR) {
        //fprintf(stderr, "Cannot handle signal=35\n");
        return 1;
    }

    while(1)
    {
        sleep(1);
    }
    return 0;
}
