#include "common.h"

int main()
{
	pid_t pid;
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid==0)
	{
		if(execl("awkexample","fork",(char*)0)<0)
			err_sys("execl error\n");
	}
	if(waitpid(pid,NULL,0)<0) err_sys("wait error\n");
	return 0;
}