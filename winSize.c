#include "common.h"

#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif

void pr_winsize(int fd)
{
	struct winsize size;
	if(ioctl(fd,TIOCGWINSZ,(char*)&size)<0)
		err_sys("TIOCGWINSZ error\n");
	printf("%d rows, %d columns\n",size.ws_row,size.ws_col);
}

void sig_winch(int signo)
{
	printf("SIGWINCH received\n");
	pr_winsize(STDIN_FILENO);
}

int main(int argc, char const *argv[])
{
	if(isatty(STDIN_FILENO)==0) exit(1);
	if(signal(SIGWINCH,sig_winch)==SIG_ERR) err_sys("signal error\n");
	pr_winsize(STDIN_FILENO);
	while(true) pause();
	return 0;
}