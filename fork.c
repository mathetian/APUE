#include "common.h"

int glob=6;
const char*buf="writetostdout\n";

int main()
{
	int var;pid_t pid;
	var=88;
	if(write(STDOUT_FILENO,buf,sizeof(buf)-1)!=sizeof(buf))
		err_sys("write error\n");
	printf("before fork\n");
	if((pid==fork())<0) err_sys("fork error\n");
	else if(pid==0) {glob++;var++;}
	else sleep(2);
	printf("pid=%d,glob=%d,var=%d\n",getpid(),glob,var);
	exit(0);
}