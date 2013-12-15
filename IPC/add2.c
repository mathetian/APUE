#include "common.h"

#define MAXLINE 10

void sig_pipe(int signo)
{
	printf("SIGPIPE caught\n");
	exit(1);
}

int main()
{
	int n,fd1[2],fd2[2];
	pid_t pid;char line[MAXLINE];
	if(signal(SIGPIPE,sig_pipe)==SIG_ERR) err_sys("signal error\n");
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid>0) 
	{
		close(fd1[0]);close(fd2[1]);
		while(fgets(line,MAXLINE,stdin)!=NULL)
		{
			n=strlen(line);printf("hello\n");
			if(write(fd1[1],line,n)!=n) err_sys("write error to pipe\n");
			if((n=read(fd2[0],line,MAXLINE))<0) err_sys("read error from pipe\n");
			if(n==0)
			{
				err_sys("read error from pipe\n");
				break;
			}
			line[n]=0;
			if(fputs(line,stdout)==EOF) err_sys("fputs error\n");
		}
		if(ferror(stdin)) err_sys("fgets error on stdin\n");
		exit(0);
	}
	else
	{
		close(fd1[1]);close(fd2[0]);
		if(fd1[0]!=STDIN_FILENO)
		{
			if(dup2(fd1[0],STDIN_FILENO)!=STDIN_FILENO) err_sys("dup2 error1\n");
			close(fd1[0]);
		}
		if(fd2[1]!=STDOUT_FILENO)
		{
			if(dup2(fd2[1],STDOUT_FILENO)!=STDOUT_FILENO) err_sys("dup2 error2\n");
			close(fd2[1]);
		}
		if(execl("./filter2","filter2",(char*)0)<0)
			err_sys("excel error\n");
	}
	exit(0);
}

