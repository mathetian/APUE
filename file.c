#include "common.h"
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	int fd;const char*buf1="abcdefghij";const char*buf2="ABCDEFGHIJ";
	if((fd=creat("file.hole",FILE_MODE))<0)
		err_sys("create error");
	if(write(fd,buf1,10)!=10)
		err_sys("buf1 write error");
	if(lseek(fd,16384,SEEK_SET)==-1)
		err_sys("lseek error");
	if(write(fd,buf2,10)!=10)
		err_sys("buf2 write error");
}