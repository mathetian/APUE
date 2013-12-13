#include "common.h"

void cleanup(void*arg)
{
	printf("cleanup %s\n",(char*)arg);
}

void*thr_fn1(void*arg)
{
	printf("thread 1 start\n");
	char a[]="thread 1 first";char b[]="thread 1 second";
	pthread_cleanup_push(cleanup,a);
	pthread_cleanup_push(cleanup,b);
	printf("thread 1 push complete\n");
	if(arg) return ((void*)1);
	pthread_cleanup_pop(0);pthread_cleanup_pop(0);
	return (void*)1;
}

void*thr_fn2(void*arg)
{
	printf("thread 2 start\n");
	char a[]="thread 2 first";char b[]="thread 2 second";
	pthread_cleanup_push(cleanup,a);
	pthread_cleanup_push(cleanup,b);
	printf("thread 2 push complete\n");
	if(arg) return ((void*)1);
	pthread_cleanup_pop(0);pthread_cleanup_pop(0);
	pthread_exit((void*)2);
}

int main()
{
	int err;pthread_t tid1,tid2;
	void *tret;
	if((err=pthread_create(&tid1,NULL,thr_fn1,(void*)1))!=0) err_quit("create1 error\n");
	if((err=pthread_create(&tid2,NULL,thr_fn2,(void*)2))!=0) err_quit("create2 error\n");
	printf("what2?");
	if((err=pthread_join(tid1,&tret))!=0) err_quit("join 1 error\n");
	printf("what1?");
	int*ddd=(int*)tret;
	printf("thread 1 exit %d\n",(int)(*ddd));
	if((err=pthread_join(tid2,&tret))!=0) err_quit("join 2 error\n");
	ddd=(int*)tret;
	printf("thread 2 exit %d\n",(int)(*ddd));
	exit(0);
}


