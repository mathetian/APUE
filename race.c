#include "common.h"

void charatatime(const char*);

int main()
{
	pid_t pid;
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid==0) charatatime("output from child\n");
	else charatatime("output from parent\n");
	exit(0);
}

void charatatime(const char*str)
{
	const char*ptr;int c;
	setbuf(stdout,NULL);
	for(ptr=str;(c=*ptr++)!=0;) putc(c,stdout);
}