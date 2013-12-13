#include "common.h"

int main()
{
	pid_t pid;
	if((pid=fork())<0) err_sys("fork error1\n");
	else if(pid!=0)
	{
		sleep(2);exit(2);
	}
	if((pid=fork())<0) err_sys("fork error2\n");
	else if(pid!=0) 
	{
		execl("/bin/dd","dd","if=/etc/termcap","of=/dev/null",NULL);
		sleep(4);abort();
	}
	if((pid=fork())) err_sys("fork error3\n");
	else if(pid!=0)
	{
		execl("/bin/dd","dd","if=/etc/termcap","of=/dev/null",NULL);
		exit(7);
	}
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid!=0)
	{
		sleep(8);exit(0);
	}
	kill(getpid(),SIGKILL);
	exit(0);
}