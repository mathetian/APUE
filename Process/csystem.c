#include "common.h"

int system(const char*cmdstring)
{
	pid_t pid;int status;
	if(cmdstring==NULL) return 1;
	if((pid=fork())<0) status=-1;
	else if(pid==0)
	{
		execl("/bin/sh","su","-c",cmdstring,(char*)0);
		_exit(127);
	}
	else
	{
		while(waitpid(pid,&status,0)<0)
		{
			if(errno!=EINTR)
			{
				status=-1;break;
			}
		}
	}
	return status;
}

int main()
{
	int status=0;
	if((status=system("date"))<0) err_sys("system() error\n");
	pr_exit(status);
	if((status=system("who; exit 44"))<0) err_sys("system() error\n");
	pr_exit(status);
	exit(0);
}