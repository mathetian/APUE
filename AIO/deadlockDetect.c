#include "common.h"

void lockabyte(const char*name,int fd,off_t offset)
{
	if(writew_lock(fd,offset,SEEK_SET,1)<0) err_sys("writew_lock error\n");
	printf("%s: got the lock, byte %ld\n",name,offset);
}

int main()
{
	int fd;pid_t pid;
	if((fd=creat("templock",FILE_MODE))<0) err_sys("creat error\n");
	if(write(fd,"ab",2)!=2) err_sys("write error!\n");
	TELL_WAIT();
	if((pid=fork())<0) err_sys("fork error\n");
	else if(pid==0)
	{
		lockabyte("child",fd,0);
		TELL_PARENT(getppid());
		WAIT_PARENT();
		lockabyte("parent",fd,1);
	}
	else
	{
		lockabyte("parent",fd,1);
		TELL_CHILD(pid);
		WAIT_CHILD();
		lockabyte("parent",fd,0);
	}
	exit(0);
}