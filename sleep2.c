#include "common.h"

jmp_buf env_alrm;

void sig_alrm(int signo)
{
	longjmp(env_alrm,1);
}

unsigned int sleep2(unsigned int nsecs)
{
	if(signal(SIGALRM, sig_alrm)==SIG_ERR) return nsecs;
	printf("ssss1\n");
	if(setjmp(env_alrm)==0)
	{
		printf("ssss2\n");
		alarm(nsecs);
		pause();
		printf("ssss3\n");
	}
	return alarm(0);
}

void sig_int(int signo)
{
	int i,j;volatile int k;
	printf("\nsig_int starting\n");
	for(i=0;i<300000;i++) for(j=0;j<4000;j++) k+=i*j;
	printf("sig_int finished\n");
}

int main()
{
	unsigned int unslept;
	if(signal(SIGINT,sig_int)==SIG_ERR) err_sys("sig_int error\n");
	unslept=sleep2(5);
	printf("sleep2 return %u\n",unslept);
	exit(0);
}