#include "common.h"

#define MAXLINE 20

int main()
{
	char line[MAXLINE];FILE*fpin;
	if((fpin=popen("./filter","r"))==NULL) err_sys("popen error\n");
	while(true)
	{
		fputs("prompt> ",stdout);
		fflush(stdout);
		if(fgets(line,MAXLINE,fpin)==NULL) break;
		if(fputs(line,stdout)==EOF) err_sys("fputs error to pipe\n");
	}
	if(pclose(fpin)==-1) err_sys("pclose error\n");
	putchar('\n');
	exit(0);
}