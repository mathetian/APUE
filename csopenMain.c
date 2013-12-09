#include "open.h"

#define BUFFSIZE 8192
#define MAXLINE 100

int main(int argc, char const *argv[])
{
	int n,fd;char buf[BUFFSIZE],line[BUFFSIZE];
	while(fgets(line,MAXLINE,stdin)!=NULL)
	{
		if(line[strlen(line)-1]=='\n')
			line[strlen(line)-1]=0;
		if((fd=csopen(line,O_RDONLY))<0) continue;
		while((n=read(fd,buf,BUFFSIZE))>0)
		{
			if(write(STDOUT_FILENO,buf,n)!=n)
				err_sys("write error\n");
		} 
		if(n<0) err_sys("read error\n");
		close(fd);
	}
	return 0;
}