#include "common.h"

pid_t *childpid=NULL;
int maxfd;

#define OPEN_MAX_GUESS	256

long
open_max(void)
{
	long openmax;
	struct rlimit rl;

	if ((openmax = sysconf(_SC_OPEN_MAX)) < 0 ||
	  openmax == LONG_MAX) {
		if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
			err_sys("can't get file limit");
		if (rl.rlim_max == RLIM_INFINITY)
			openmax = OPEN_MAX_GUESS;
		else
			openmax = rl.rlim_max;
	}
	return(openmax);
}


FILE* popen(const char*cmdstring, const char*type)
{
	int i,pfd[2];
	pid_t pid;FILE*fp;
	/**only allow r or w**/
	if((type[0]!='r'&&type[0]!='w')||type[1]!=0)
	{
		errno=EINVAL;return NULL;
	}
	if(childpid==NULL)
	{
		/**allocate zeroed out array for child pids**/
		maxfd=open_max();
		if((childpid=(pid_t*)calloc(maxfd,sizeof(pid_t)))==NULL) return NULL;
	}
	if(pipe(pfd)<0) return NULL;
	if((pid=fork())<0) return NULL;
	else if(pid==0) 
	{
		if(*type=='r')
		{
			close(pfd[0]);
			if(pfd[1]!=STDOUT_FILENO)
			{
				dup2(pfd[1],STDOUT_FILENO);
				close(pfd[1]);
			}
		}
		else
		{
			close(pfd[1]);
			if(pfd[0]!=STDIN_FILENO)
			{
				dup2(pfd[0],STDIN_FILENO);
				close(pfd[0]);
			}
		}
		for(i=0;i<maxfd;i++) if(childpid[i]>0) close(i);
		execl("/bin/sh","sh","-c",cmdstring,(char*)0);
		_exit(127);
	}
	if(*type=='r')
	{
		close(pfd[1]);
		if((fp=fdopen(pfd[0],type))==NULL)
			return NULL;
	}
	else
	{
		close(pfd[0]);
		if((fp=fdopen(pfd[1],type))==NULL)
			return NULL;
	}
	childpid[fileno(fp)]=pid;
	return fp;
}

int pclose(FILE*fp)
{
	int fd,stat;pid_t pid;
	if(childpid==NULL) 
	{
		errno=EINVAL;
		return -1;
	}
	fd=fileno(fp);
	if((pid=childpid[fd])==0)
	{
		errno=EINVAL;
		return -1;
	}
	childpid[fd]=0;
	if(fclose(fp)==EOF) return -1;
	while(waitpid(pid,&stat,0)<0)
	{
		if(errno!=EINTR) return -1;
	}
	return stat;
}
