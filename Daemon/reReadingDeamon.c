#include "common.h"

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

sigset_t mask;

void reread()
{

}

void*thr_fn(void*arg)
{
	int err,signo;
	while(true)
	{
		if((err=sigwait(&mask,&signo))!=0)
		{
			syslog(LOG_ERR,"sigwait failed");
			exit(1);
		}
		switch(signo)
		{
		case SIGHUP:
			syslog(LOG_INFO,"Re-reading configuration file");
			reread();
			break;
		case SIGTERM:
			syslog(LOG_INFO,"got SIGTERM; exiting");
			exit(0);
		default:
			syslog(LOG_INFO,"unexpected signal %d\n",signo);
		}
	}
	return 0;
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
	int err;pthread_t tid;
	char*cmd;struct sigaction sa;
	if((cmd=strrchr(argv[0],'/'))==NULL) cmd=argv[0];
	else cmd++;
	/**Become a daemon**/
	printf("%s\n",cmd);
	daemonize(cmd);printf("%s\n",cmd);
	/**Make sure only one copy**/
	syslog(LOG_ERR,"deamon already_running\n");
	printf("fdfasdfdsafdsfsdfdsafsdf\n");
	if(already_running())
	{
		syslog(LOG_ERR,"deamon already_running\n");
		exit(1);
	}
	/**Restore**/
	sa.sa_handler=SIG_DFL;
	sigemptyset(&sa.sa_mask);
	if((err=pthread_sigmask(SIG_BLOCK,&mask,NULL))!=0) err_quit("SIG_BLOCK error\n");
	/**Create Thread**/
	if((err=pthread_create(&tid,NULL,thr_fn,0))!=0) err_quit("can't create thread\n");
	exit(0);
}