#include "common.h"

void sig_hup(int);
void pr_ids(const char*);

int main()
{
	char c;pid_t pid;
	pr_ids("parent");
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid>0)
	{
		sleep(5);exit(0);
	}
	else
	{
		pr_ids("child1");
		signal(SIGHUP,sig_hup);
		/**SIGTSTP: ctrl+z
		   SIGHUP:  close terminal
		**/
		kill(getpid(),SIGTSTP);
		pr_ids("child2");
		if(read(STDIN_FILENO,&c,1)!=1)
			printf("read error from controlling TTY, errno = %d\n",errno);
		exit(0);
	}
}

void sig_hup(int signo)
{
	printf("SIGHUP received, pid: %d\n",getpid());
}

void pr_ids(const char *name)
{
	printf("%s: pid = %d, ppid = %d, pgrp = %d, tpgrp = %d\n",name,getpid(),getppid(),getpgrp(),tcgetpgrp(STDIN_FILENO));
}