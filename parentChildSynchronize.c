#include "common.h"

int pfd1[2],pfd2[2];

void TELL_WAIT()
{
	if(pipe(pfd2)<0||pipe(pfd2)<0) err_sys("pipe error\n");
}

void TELL_PARENT()
{
	if(write(pfd2[1],"c",1)!=1) err_sys("write error\n");
}

void WAIT_PARENT()
{
	char c;
	if(read(pfd1[0],&c,1)!=1) err_sys("read error\n");
	if(c!='p') err_quit("incorrect data\n");
}

void TELL_CHILD(pid_t pid)
{
	if(write(pfd1[1],"p",1)!=1) err_sys("write error\n");
}

void WAIT_CHILD()
{
	char c;
	if(read(pfd2[0],&c,1)!=1) err_sys("read error\n");
	if(c!='c') err_quit("incorrect data\n");
}