#include "common.h"

#define MAXLINE 100
#define	DEF_PAGER	"/bin/more"

int main(int argc,char*argv[])
{
	int n;int fd[2];
	pid_t pid;const char*pager,*argv0;
	char line[MAXLINE];FILE*fp;
	if(argc!=2) err_quit("usage\n");
	if((fp=fopen(argv[1],"r"))==NULL) err_quit("can't open\n");
	if(pipe(fd)<0) err_sys("pipe error\n");
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid>0)
	{
		close(fd[0]);
		while(fgets(line,MAXLINE,fp)!=NULL)
		{
			n=strlen(line);
			if(write(fd[1],line,n)!=n) err_sys("write pipe error\n");
		}
		if(ferror(fp)) err_sys("fgets error\n");
		close(fd[1]);
		if(waitpid(pid,NULL,0)<0) err_sys("waitpid error\n");
		exit(0);
	}
	else
	{
		close(fd[1]);
		if(fd[0]!=STDIN_FILENO)
		{
			if(dup2(fd[0],STDIN_FILENO)!=STDIN_FILENO) err_sys("dup2 error\n");
			close(fd[0]);
		}
		if((pager=getenv("PAGER"))==NULL) pager=DEF_PAGER;
		if((argv0=strrchr(pager,'/'))!=NULL) argv0++;
		else argv0=pager;printf("%s %s\n",pager,argv0);
		if(execl(pager,argv0,(char*)0)<0) err_sys("execl error\n");
	}
	exit(0);
}