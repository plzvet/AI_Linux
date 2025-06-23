#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "signalprint.h"

void handler(int sig){
	//sigset_t sigset;
    sigset_t block_all_except; // 새로운 시그널 집합
    sigfillset(&block_all_except); // 시그널 집합 모두 1로 설정 --> 모든 시그널을 차단했다는 뜻
    sigdelset(&block_all_except, SIGINT); // 특정 시그널 삭제 SIGINT --> 차단해제
    sigdelset(&block_all_except, SIGQUIT); // 특정 시그널 삭제 SIGQUIT --> 차단해제
    sigprocmask(SIG_SETMASK, &block_all_except, NULL); // pending 되었던 시그널이 전달됨.(내가 차단 푼거만)

    sigprocmask(SIG_SETMASK, NULL, &sigset);
	
    printf("blocked signal set : ");
	print_sigset_t(&block_all_except);
	for(int i=1; i<=3; i++) {
		printf("handler_%d!!\n", i);
		sleep(1);
	    }
}


int main(void){
	struct sigaction act, oldact;
	
	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGQUIT); // 핸들러가 실행되는 동안 임시로 차단
	sigaddset(&act.sa_mask, SIGINT); // INT, QUIT 임시차단
	// act.sa_flags=SA_NODEFER;
	act.sa_flags=SA_RESTART;
	// act.sa_flags=SA_RESETHAND;
	sigaction(SIGINT, &act, &oldact);
    sigaction(SIGQUIT, &act, NULL);	
	for(int i=1; ; i++){
		printf("signal test --- %d\n", i);
		sleep(2);
	}
	return 0;
}
