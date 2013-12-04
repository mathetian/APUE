#ifndef _COMMON
#define _COMMON

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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


#endif
