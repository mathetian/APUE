#include "common.h"

struct foo{
	int f_count;
	pthread_mutex_t f_lock;
};

struct foo * foo_alloc()
{
	struct foo*fp;
	if((fp=(struct foo*)malloc(sizeof(struct foo)))!=NULL)
	{
		fp->f_count=1;
		if(pthread_mutex_init(&fp->f_lock,NULL)!=0)
		{
			free(fp);return NULL;
		}
	}
	return fp;
}

void foo_hold(struct foo *fp)
{
	pthread_mutex_lock(&fp->f_lock);
	fp->f_count++;
	pthread_mutex_unlock(&fp->f_lock);
}

void foo_free(struct foo *fp)
{
	pthread_mutex_lock(&fp->f_lock);
	if(--fp->f_count==0)
	{
		pthread_mutex_unlock(&fp->f_lock);
		pthread_mutex_destroy(&fp->f_lock);
		free(fp);
	}
	else pthread_mutex_unlock(&fp->f_lock);
}