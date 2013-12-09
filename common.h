#ifndef _COMMON
#define _COMMON

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>
#include <syslog.h>
#include <termios.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWOTH)

void err_sys(const char*msg)
{
	fputs(msg,stderr);
}

void err_quit(const char*msg)
{
	fputs(msg,stderr);
	exit(0);
}

#ifdef	PATH_MAX
static int	pathmax = PATH_MAX;
#else
static int	pathmax = 0;
#endif

#define SUSV3	200112L

static long	posix_version = 0;
#define	PATH_MAX_GUESS	1024

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

#define read_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len)
#define readw_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLKW,F_RDLCK, offset, whence, len)
#define write_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)
#define writew_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len)
#define un_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLK,F_UNLOCK,offset,whence,len)
#define is_read_lockable(fd,offset,whence,len) \
			lock_test(fd,F_RDLCK,offset,whence,len) == 0
#define is_write_lockable(fd,offset,whence,len) \
			lock_test(fd,F_WRLCK,offset,whence,len) == 0
#define lockfile(fd) write_lock(fd,0,SEEK_SET,0)
#endif

