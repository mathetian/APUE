#include "common.h"

#define MAXLINE 100
void sig_pipe(int);

int s_pipe(int fd[2])
{
	return(socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}

int main(int argc, char const *argv[])
{
	int n;int fd[2];pid_t pid;
	char line[MAXLINE];
	if(signal(SIGPIPE,sig_pipe)==SIG_ERR) err_sys("signal error\n");
	if(s_pipe(fd)<0) err_sys("pipe error\n");
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid>0)
	{
		close(fd[1]);
		while(fgets(line,MAXLINE,stdin)!=NULL)
		{
			n=strlen(line);
			if(write(fd[0],line,n)!=n) err_sys("write error to pipe\n");
			if((n=read(fd[0],line,MAXLINE))<0) err_sys("read error from pipe\n");
			if(n==0) 
			{
				err_sys("child closed pipe\n");break;
			}
			line[n]=0;
			if(fputs(line,stdout)==EOF) err_sys("fputs error\n");
		}
		if(ferror(stdin)) err_sys("fgets error on stdin\n");
		exit(0);
	}
	else
	{
		close(fd[0]);
		if(fd[1]!=STDIN_FILENO&&dup2(fd[1],STDIN_FILENO)!=STDIN_FILENO)
			err_sys("dup2 error\n");
		if(fd[1]!=STDOUT_FILENO&&dup2(fd[1],STDOUT_FILENO)!=STDOUT_FILENO)
			err_sys("dup2 error to stdout\n");
		if(execl("./filter2","filter2",(char*)0)<0) err_sys("execl error\n");
	}
	exit(0);
}

void sig_pipe(int signo)
{
	printf("SIGPIPE caught\n");
	exit(1);
}