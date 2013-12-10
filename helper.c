#include "common.h"

void err_sys(const char*msg)
{
	fputs(msg,stderr);
}

void err_quit(const char*msg)
{
	fputs(msg,stderr);
	exit(0);
}

char * path_alloc(int *sizep)
{
	char	*ptr;
	int	size;

	if (posix_version == 0) posix_version = sysconf(_SC_VERSION);

	if (pathmax == 0) {	
		errno = 0;
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
			if (errno == 0)
				pathmax = PATH_MAX_GUESS;
			else
				err_sys("pathconf error for _PC_PATH_MAX");
		} else {
			pathmax++;
		}
	}
	if (posix_version < SUSV3)
		size = pathmax + 1;
	else
		size = pathmax;

	if ((ptr = (char*)malloc(size)) == NULL)
		err_sys("malloc error for pathname");

	if (sizep != NULL)
		*sizep = size;
	return(ptr);
}

void pr_exit(int status)
{
	if(WIFEXITED(status))
		printf("normal termination, exit status = %d\n",WEXITSTATUS(status));
	else if(WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",WTERMSIG(status),
#ifdef WCOREDUMP
	WCOREDUMP(status)?" (core file generated)":"");
#else 
	"");
#endif
	else if(WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",WSTOPSIG(status));
}

int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len)
{
	struct flock lock;
	lock.l_type=type;lock.l_start=offset;
	lock.l_whence=whence;lock.l_len=len;
	return fcntl(fd,cmd,&lock);
}

pid_t lock_test(int fd,int type,off_t offset,int whence,off_t len)
{
	struct flock lock;
	lock.l_type=type;lock.l_start=offset;
	lock.l_whence=whence;lock.l_len=len;
	if(fcntl(fd,F_GETLK,&lock)<0) err_sys("fcntl error\n");
	if(lock.l_type==F_UNLCK) return 0;
	return lock.l_pid;
}