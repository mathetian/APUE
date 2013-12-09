#include "common.h"
#include <sys/wait.h>

#define PAGER "more"
#define MAXLINE 50

int main(int argc,char*argv[])
{
	char line[MAXLINE];FILE*fpin,*fpout;
	if(argc!=2) err_quit("usage error\n");
	if((fpin=fopen(argv[1],"r"))==NULL) err_sys("can't open it\n");
	if((fpout=popen(PAGER,"w"))==NULL) err_sys("popen error\n");
	while(fgets(line,MAXLINE,fpin)!=NULL)
	{
		printf("%s",line);
		if(fputs(line,fpout)==EOF) err_sys("fputs error to pipe\n");
	}
	if(ferror(fpin)) err_sys("fgets error\n");
	if(pclose(fpout)==-1) err_sys("pclose error\n");
	exit(0);
}