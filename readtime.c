#include "common.h"

void sig_alrm(int);
jmp_buf env_alrm;

#define MAXLINE 10
int main()
{
	int n;char line[MAXLINE];
	if(signal(SIGALRM,sig_alrm)==SIG_ERR) err_sys("SIGALRM error\n");
	if(setjmp(env_alrm)!=0) err_quit("read timeout\n");
	alarm(10);
	if((n=read(STDIN_FILENO,line,MAXLINE))<0) err_sys("read error\n");
	write(STDOUT_FILENO,line,n);
	exit(0);
}

void sig_alrm(int signo)
{
	printf("sig_alrm: %d\n",signo);
}