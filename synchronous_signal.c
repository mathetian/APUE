#include "common.h"

int quitflag;sigset_t mask;

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait1=PTHREAD_COND_INITIALIZER;

void* thr_fn(void*arg)
{
	int err,signo;
	while(true)
	{
		if((err=sigwait(&mask,&signo))!=0) err_quit("sigwait failed\n");
		switch(signo)
		{
		case SIGINT: 
			printf("\ninterrupt\n");
			break;
		case SIGQUIT:
			printf("quit\n");
			pthread_mutex_lock(&lock);
			quitflag=1;printf("quit2\n");
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&wait1);
			return 0;
		default:
			printf("unexpected signal %d\n",signo);
			exit(1);
		}
	}
}

int main()
{
	int err;sigset_t oldmask;pthread_t tid;
	sigemptyset(&mask);sigaddset(&mask,SIGINT);sigaddset(&mask,SIGQUIT);
	if((err=pthread_sigmask(SIG_BLOCK,&mask,&oldmask))!=0) err_quit("SIGBLOCK error\n");
	if((err=pthread_create(&tid,NULL,thr_fn,0))!=0) err_quit("can't create thread\n");
	pthread_mutex_lock(&lock);
	printf("enter %d\n",quitflag);
	while(quitflag==0) pthread_cond_wait(&wait1,&lock);
	pthread_mutex_unlock(&lock);
	quitflag=0;
	if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0) err_sys("SIG_SETMASK error\n");
	exit(0);
}