#include "common.h"

void sig_quit(int);

int main()
{
	sigset_t newmask,oldmask,pendmask;
	if(signal(SIGQUIT,sig_quit)==SIG_ERR) err_sys("can't catch SIGQUIT\n");
	sigemptyset(&newmask);sigaddset(&newmask,SIGQUIT);
	if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0) err_sys("SIG_BLOCK error\n");
	sleep(5);if(sigpending(&pendmask)<0) err_sys("sigpending error\n");
	if(sigismember(&pendmask,SIGQUIT)) printf("\nSIGQUIT pending\n");
	if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0) err_sys("SIGSETMASK error\n");
	printf("SIGQUIT unblocked\n");
	sleep(5);
	exit(0);
}

void sig_quit(int signo)
{
	printf("canught SIGQUIT\n");
	if(signal(SIGQUIT,SIG_DFL)==SIG_ERR) err_sys("can't reset SIGQUIT\n");
}

#define SIGBAD(signo) ((signo <= 0)||(signo) >= NSIG)
#define sigemptyset(ptr) (*(ptr)=0)
#define sigfillset(ptr) (*(ptr)=~(sigset_t)0,0);

int sigaddset(sigaddset *set,int signo)
{
	if(SIGBAD(signo)) {errno=EINVAL;return -1;}
	*set |= 1<<(signo-1);
	return 0;
}

int sigdelset(sigset_t *set,int signo)
{
	if(SIGBAD(signo)) {errno=EINVAL;return -1;}
	*set &= ~(1<<(signo-1));
	return 0;
}

ing sigismember(sigset_t *set,int signo)
{
	if(SIGBAD(signo)) {errno=EINVAL;return -1;}
	return (*set & (1<<(signo-1)))!=0
}



