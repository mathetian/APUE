#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <syslog.h>
#include <pthread.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/mman.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWOTH)

#define SUSV3			200112L
#define	MAXLINE			4096
#define	PATH_MAX_GUESS	1024

extern int pathmax;

extern long	posix_version;

#define read_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len)
#define readw_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLKW,F_RDLCK,offset,whence,len)
#define write_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)
#define	writew_lock(fd, offset, whence, len) \
			lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len)
#define un_lock(fd,offset,whence,len) \
			lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)
#define is_read_lockable(fd,offset,whence,len) \
			lock_test(fd,F_RDLCK,offset,whence,len) == 0
#define is_write_lockable(fd,offset,whence,len) \
			lock_test(fd,F_WRLCK,offset,whence,len) == 0
#define lockfile(fd) write_lock(fd,0,SEEK_SET,0)

int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len);

pid_t lock_test(int fd,int type,off_t offset,int whence,off_t len);

extern void   err_sys(const char *fmt, ...);

void   err_quit(const char *fmt, ...);
void   err_ret(const char *fmt, ...);
void   err_sys(const char *fmt, ...);
void   err_exit(int error, const char *fmt, ...);
void   err_dump(const char *fmt, ...);
void   err_msg(const char *fmt, ...);
void   err_quit(const char *fmt, ...);


char  *path_alloc(int *sizep);
void   pr_exit(int status);
void   setfl(int fd,int flags);

void   TELL_WAIT(void);
void   TELL_PARENT(pid_t pid);
void   WAIT_PARENT(void);
void   TELL_CHILD(pid_t pid);
void   WAIT_CHILD(void);

ssize_t readn(int fd,void*ptr,size_t n);
ssize_t writen(int fd,void*ptr,size_t n);
void    set_fl(int fd, int flags);
void    clr_fl(int fd, int flags);

void    daemonize(const char*cmd);

#ifndef true
#define true 1
#define false 0
#endif

#endif

