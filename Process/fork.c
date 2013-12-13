#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int glob=6;
const char*buf="writetostdout\n";

int main(int argc,char*argv[])
{
	int var;pid_t pid;int i;
	var=88;
	for(i=0;i<argc;i++) printf("%s ",argv[i]);
	printf("\n");
	if(write(STDOUT_FILENO,buf,sizeof(buf)-1)!=sizeof(buf))
		printf("write error\n");
	printf("before fork\n");
	if((pid==fork())<0) printf("fork error\n");
	else if(pid==0) {glob++;var++;}
	else sleep(2);
	printf("pid=%d,glob=%d,var=%d\n",getpid(),glob,var);
	exit(0);
}