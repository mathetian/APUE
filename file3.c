#include "common.h"
#include <fcntl.h>

void setfl(int fd,int flags)
{
	int val;
	if((val=fcntl(fd,F_GETFL,0))<0)
		err_sys("fcntl F_GETFL error\n");
	val|=flags;
	if(fcntl(fd,F_SETFL,val)<0)
		err_sys("fcntl F_SETFL error\n");
}


int main(int argc, char*argv[])
{
	int val;
	if(argc!=2) err_quit("usgage error\n");
	if((val=fcntl(atoi(argv[1]),F_GETFL,0))<0)
		err_sys("fcntl error for fd\n");
	switch(val&O_ACCMODE)
	{
	case O_RDONLY:
		printf("read only");break;
	case O_WRONLY:
		printf("write only");break;
	case O_RDWR:
		printf("both read and write");break;
	default:
		err_sys("unknown access mode");
	}
	if(val&O_APPEND) printf(", append");
	if(val&O_NONBLOCK) printf(", nonblocking");
#if defined(O_SYNC)
	if(val&O_SYNC) printf(", synchronous write");
#endif
#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC)
	if(val&O_FSYNC) printf(", synchronous writes");
#endif
	putchar('\n');
	return 0;
}