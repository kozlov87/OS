#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
	
const int TIME = 10;

volatile sig_atomic_t sig_status = 0;

void sig_handler(int signo, siginfo_t *siginfo, void *context) {
	sig_status = 1;
    pid_t sender_pid = siginfo->si_pid;

	printf("signal %d from: %d\n", signo, (int)sender_pid);
	exit(signo);
}

int main() {
    struct sigaction sigact;
    sigact.sa_sigaction = *sig_handler;
    sigact.sa_flags |= SA_SIGINFO;
    
    sigset_t block_mask;
	sigfillset(&block_mask);

	int count = 2;
	int cant_handle[] = {SIGKILL, 19};
   
    sigact.sa_mask = block_mask;
	int i;
	for(i = 1; i < 32; ++i) {
		int j;
		char isOk = 1;
		for(j = 0; j < count; ++j) {
			if(i == cant_handle[j]) {
				isOk = 0;
				break;
			}
		} 
		if (isOk && sigaction(i, &sigact, NULL) != 0) {
        		printf("%d error\n", i);
       		 	return errno;
    		}
	}
	 
    sleep(TIME);

	sigprocmask(SIG_BLOCK, &sigact.sa_mask, 0);

	if(!sig_status) {
    	printf("No signals were caught\n");
	}
    return 0;
}

