#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAXLINE 100

int main()
{
	char buf[MAXLINE];pid_t pid;int status;
	printf("%% ");
	while(fgets(buf,MAXLINE,stdin)!=NULL)
	{
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]=0;
		if((pid=fork())<0) printf("fork error\n");
		else if(pid==0)	execlp(buf,buf,(char*)0);
		if((pid==waitpid(pid,&status,0))<0)
			printf("wait error");
		printf("%% ");
	}
	return 0;
}