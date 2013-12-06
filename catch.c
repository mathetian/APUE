#include "common.h"

void sig_usr(int);

int main()
{
	if(signal(SIGUSR1,sig_usr)==SIG_ERR) err_sys("can't catch SIG_ERR\n");
	if(signal(SIGUSR2,sig_usr)==SIG_ERR) err_sys("can't catch SIG_ERR\n");
	for(;;) pause();
	exit(0);
}

void sig_usr(int signo)
{
	if(signo==SIGUSR1) printf("received SIGUSR1\n");
	else if(signo==SIGUSR2) printf("received SIGUSR2\n");
	else err_sys("received signal\n");
}