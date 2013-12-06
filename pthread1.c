#include "common.h"
#include <pthread.h>

void printtide(const char*s)
{
	printf("%s pid %u tid %u (0x%x)\n",s,(unsigned int)getpid(),(unsigned int)pthread_self(),(unsigned int)pthread_self());
}

void*thr_fn(void*arg)
{
	printtide("new thread: ");
	return 0;
}

int main()
{
	pthread_t ntid;int err;
	if((err=pthread_create(&ntid,NULL,thr_fn,NULL))!=0) err_quit("create error\n");
	printtide("main thread: ");
	sleep(1);exit(0);
	return 0;
}