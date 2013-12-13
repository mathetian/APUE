#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
int main()
{
	const char*env_init[]={"USER=unknown","PATH=/tmp",NULL};
	pid_t pid;
	if((pid=fork())<0) printf("fork error\n");
	else if(pid==0) 
	{
		if(execle("fork","fork",(char*)0,env_init)<0)
			printf("execle error\n");
	}
	if(waitpid(pid,NULL,0)<0) printf("wait error\n");
	if((pid=fork())<0) printf("fork2 error\n");
	else if(pid==0)
	{
		if(execlp("exit","exit",(char*)0)) printf("execlp error\n");
	}
	exit(0);
}