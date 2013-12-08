#include "common.h"

int makethread(void*(*fn)(void*),void*arg)
{
	int err;pthread_t tid;
	pthread_attr_t attr;
	if((err=pthread_attr_init(&attr))!=0) return err;
	if((err=pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED))==0)
		err=pthread_create(&tid,&attr,fn,arg);
	pthread_attr_destroy(&attr);
	return err;
}

struct to_info{
	void (*to_fn)(void*);void *to_arg;
	struct timespec to_wait;
};

#define SECTONSEC 1000000000
#define USECTONSEC 1000

void* timeout_helper(void*arg)
{
	struct to_info *tip=(struct to_info *)arg;
	nanosleep(&tip->to_wait,NULL);
	(*tip->to_fn)(tip->to_arg);
	return 0;
}

void timeout(const struct timespec *when, void (*func)(void *), void*arg)
{
	struct timespec now;
	struct timeval tv;
	struct to_info *tip;
	int err;
	gettimeofday(&tv,NULL);
	now.tv_sec=tv.tv_sec;
	now.tv_nsec=tv.tv_usec*USECTONSEC;
	if((when->tv_sec > now.tv_sec)||(when->tv_sec==now.tv_sec&&when->tv_nsec>now.tv_nsec))
	{
		tip=(struct to_info*)malloc(sizeof(struct to_info));
		if(tip!=NULL)
		{
			tip->to_fn=func;
			tip->to_arg=arg;
			tip->to_wait.tv_sec=when->tv_sec-now.tv_sec;
			if(when->tv_nsec>=now.tv_nsec)
				tip->to_wait.tv_nsec=when->tv_nsec-now.tv_nsec;
			else
			{
				tip->to_wait.tv_sec--;
				tip->to_wait.tv_nsec=SECTONSEC-now.tv_nsec+when->tv_nsec;
			}
			if((err=makethread(timeout_helper,(void*)tip))==0) return;
		}
	}	
	(*func)(arg);
}

pthread_mutexattr_t attr;
pthread_mutex_t mutex;

void retry(void*arg)
{
	pthread_mutex_lock(&mutex);
	/**perform retry steps*/
	pthread_mutex_unlock(&mutex);
}

int main()
{
	int err,condition=1,arg;
	struct timespec when;
	if((err=pthread_mutexattr_init(&attr))!=0) err_quit("pthread_mutexattr_init failed\n");
	if((err=pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE))!=0) err_quit("can't set PTHREAD_MUTEX_RECURSIVE\n");
	/**...**/
	pthread_mutex_lock(&mutex);
	/**...*/
	if(condition)
	{
		/**calculate target time "when"**/
		timeout(&when,retry,(void*)arg);
	}
	/**...**/
	pthread_mutex_unlock(&mutex);
	/**...**/
	exit(0);
}