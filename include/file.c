#include "common.h"

ssize_t readn(int fd,void*ptr,size_t n)
{
	size_t nleft;ssize_t nread;
	nleft=n;
	while(nleft>0)
	{
		if((nread=read(fd,ptr,nleft))<0)
		{
			if(nleft==n) return -1;
			else break;
		}
		else if(nread==0) break;
		nleft-=nread;
		ptr+=nread;
	}
	return n-nleft;
}

ssize_t writen(int fd,void*ptr,size_t n)
{
	size_t nleft;ssize_t nwritten;
	nleft=n;
	while(nleft>0)
	{
		if((nwritten=write(fd,ptr,nleft))<0) 
		{
			if(nleft==n) return -1;
			else break;
		}
		else if(nwritten==0) break;
		nleft-=nwritten;
		ptr+=nwritten;
	}
	return n-nleft;
}

void set_fl(int fd, int flags)
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val |= flags;		/* turn on flags */

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}

void clr_fl(int fd, int flags)
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val &= ~flags;		/* turn flags off */

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}