#include "common.h"

void daemonize(const char*cmd)
{
	int i,fd0,fd1,fd2;
	pid_t pid;
	struct rlimit r1;
	struct sigaction sa;
	/**step 1**/
	umask(0);
	/**step 2**/
	if(getrlimit(RLIMIT_NOFILE,&r1)<0)
		err_quit("can't get file limit\n");
	/**step 3**/
	if((pid=fork())<0) err_quit("can't fork\n");
	else if(pid!=0) exit(0);
	setsid();
	/**step 4*/
	sa.sa_handler=SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;
	if(sigaction(SIGHUP,&sa,NULL)<0) err_quit("can't ignore SIGHUP\n");
	if((pid=fork())<0) err_quit("can't fork\n");
	else if(pid!=0) exit(0);
	/**step 5**/
	if(chdir("/")<0) err_quit("can't change directory to /\n");
	/**step 6**/
	if(r1.rlim_max==RLIM_INFINITY) r1.rlim_max=1024;
	for(i=0;i<r1.rlim_max;i++) close(i);
	/**step 7**/
	fd0=open("/dev/null",O_RDWR);
	fd1=dup(0);fd2=dup(0);
	/**step 8**/
	openlog(cmd,LOG_CONS,LOG_DAEMON);
	if(fd0!=0||fd1!=1||fd2!=2)
	{
		syslog(LOG_ERR,"unexpected file description %d %d %d",fd0,fd1,fd2);
		exit(1);
	}
}