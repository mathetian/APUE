#include "common.h"

sigset_t mask;

void reread()
{

}
int
lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
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
#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)


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

void daemonize(const char*cmd)
{
	int i,fd0,fd1,fd2;
	pid_t pid;
	struct rlimit r1;
	struct sigaction sa;
	/**step 1**/
	umask(0);
	/**step 2**/
	if(getrlimit(RLIMIT_NOFILE,&r1)<0) err_quit("can't get file limit\n");
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