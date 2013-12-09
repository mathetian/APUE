#include "common.h"
#include <stropts.h>
#include <unistd.h>

int isastream(int fd)
{
	return ioctl(fd,I_CANPUT,0)!=-1;
}

int main(int argc,char*argv[])
{	
	int i,fd;
	for(i=0;i<argc;i++)
	{
		if((fd=open(argv[1],O_RDONLY))<0) err_quit("can't open\n");
		if(isastream(fd)==0) printf("%s, not a stream\n",argv[i]);
		else printf("%s: streams device",argv[i]);
	}

	exit(0);
}