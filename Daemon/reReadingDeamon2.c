#include "common.h"

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

sigset_t mask;

void reread()
{

}

void sigterm(int signo)
{
	syslog(LOG_INFO,"got SIGTERM; exiting");
	exit(0);
}

void sighup(int signo)
{
	syslog(LOG_INFO,"Re-reading configuration file");
	reread();
}

int already_running()
{
	int fd;char buf[16];
	if((fd=open(LOCKFILE,O_RDWR|O_CREAT,LOCKMODE))<0)
	{
		syslog(LOG_ERR,"can't open %s: %s",LOCKFILE,strerror(errno));
		exit(1);
	}
	if(lockfile(fd)<0)
	{
		if(errno==EACCES||errno==EAGAIN)
		{
			close(fd);return 1;
		}
		syslog(LOG_ERR,"can't lock %s: %s",LOCKFILE,strerror(errno));
		exit(1);
	}
	ftruncate(fd,0);
	sprintf(buf,"%d",(int)getpid());
	write(fd,buf,strlen(buf)+1);
	return 0;
}

int main(int arg,char*argv[])
{
	char*cmd;struct sigaction sa;
	if((cmd=strrchr(argv[0],'/'))==NULL) cmd=argv[0];
	else cmd++;
	/**Become a daemon**/
	daemonize(cmd);
	/**Make sure only one copy**/
	syslog(LOG_ERR,"deamon already_running\n");
	printf("fdfasdfdsafdsfsdfdsafsdf\n");
	if(already_running())
	{
		syslog(LOG_ERR,"deamon already_running\n");
		exit(1);
	}
	/**Restore**/
	sa.sa_handler=sigterm;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGHUP);
	sa.sa_flags=0;
	if(sigaction(SIGTERM,&sa,NULL)<0)
	{
		syslog(LOG_ERR,"can't catch SIGTERM: %s",strerror(errno));
		exit(1);
	}
	sa.sa_handler=sighup;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGTERM);
	sa.sa_flags=0;
	if(sigaction(SIGHUP,&sa,NULL)<0)
	{
		syslog(LOG_ERR,"can't catch SIGHUP: %s",strerror(errno));
		exit(1);
	}
	exit(0);
}