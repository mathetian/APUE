#include <stdio.h>
#include <unistd.h>

#ifndef BUFFSIZE
#define BUFFSIZE 4096
#endif

int main()
{
	int n;char buf[BUFFSIZE];
	while((n=read(STDIN_FILENO, buf, BUFFSIZE))>0)
	{
		if(write(STDOUT_FILENO,buf,n)!=n)
		{
			printf("error1\n");
			break;
		}
		if(n<0)
		{
			printf("error2\n");
			break;
		}
	}
	return 0;
}