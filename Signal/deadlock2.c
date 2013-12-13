#include "common.h"

#define NHASH 29
#define HASH(fp) (((unsigned long)fp)%NHASH)

struct foo *fh[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo{
	int f_count, f_id;
	pthread_mutex_t f_lock;
	struct foo *f_next;
};

struct foo* foo_alloc()
{
	struct foo *fp;int idx;
	if((fp=(struct foo*)malloc(sizeof(struct foo)))!=NULL)
	{
		fp->f_count=1;
		if(pthread_mutex_init(&fp->f_lock,NULL)!=0)
		{
			free(fp);return NULL;
		}
		idx=HASH(fp);
		pthread_mutex_lock(&hashlock);
		fp->f_next=fh[idx];
		fh[idx]=fp->f_next;
		pthread_mutex_lock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
		/**continue initialization**/
		pthread_mutex_unlock(&fp->f_lock);
	}
	return fp;
}

void foo_hold(struct foo *fp)
{
	pthread_mutex_lock(&fp->f_lock);
	fp->f_count++;
	pthread_mutex_unlock(&fp->f_lock);
}

struct foo * foo_find(int id)
{
	struct foo *fp;
	int idx;idx=HASH(fp);
	pthread_mutex_lock(&hashlock);
	for(fp=fh[idx];fp!=NULL;fp=fp->f_next)
	{
		if(fp->f_id==id)
		{
			foo_hold(fp);
			break;
		}
	}
	pthread_mutex_unlock(&hashlock);
	return fp;
}

void foo_free(struct foo *fp)
{
	struct foo *tfp;int idx;
	pthread_mutex_lock(&hashlock);
	if(--fp->f_count==0)
	{
		idx=HASH(fp);tfp=fh[idx];
		if(tfp==fp) fh[idx]=fp->f_next;
		else 
		{
			while(tfp->f_next!=fp)
				tfp=tfp->f_next;
			tfp->f_next=fp->f_next;
		}
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destory(&fp->f_lock);
		free(fp);
	}
	else pthread_mutex_unlock(&hashlock);
}