#include "common.h"

int main()
{
	const char*env_init[]={"USER=unknown","PATH=/tmp",NULL};
	pid_t pid;
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid==0) 
	{
		if(execle("fork","fork123",(char*)0,env_init)<0)
			err_sys("execle error\n");
	}
	if(waitpid(pid,NULL,0)<0) err_sys("wait error\n");
	if((pid=fork())<0) err_sys("fork2 error\n");
	else if(pid==0)
	{
		if(execlp("exit","exit",(char*)0)) err_sys("execlp error\n");
	}
	exit(0);
}