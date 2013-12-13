#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define BUFSIZE 1024

void sig_tstp(int);

int main()
{
	int n;char buf[BUFSIZE];
	if(signal(SIGTSTP,SIG_IGN)==SIG_DFL) signal(SIGTSTP,sig_tstp);
	while((n=read(STDIN_FILENO,buf,BUFSIZE))>0)
		if(write(STDOUT_FILENO,buf,n)!=n) printf("write error\n");
	if(n<0) printf("read error\n"); 
	exit(0);
}

void sig_tstp(int signo)
{
	sigset_t mask;printf("hello\n");
	sigemptyset(&mask);
	sigaddset(&mask,SIGTSTP);
	sigprocmask(SIG_UNBLOCK,&mask,NULL);
	signal(SIGTSTP,SIG_DFL);
	kill(getpid(),SIGTSTP);
	signal(SIGTSTP,sig_tstp);
}