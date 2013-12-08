#include "common.h"

pthread_key_t key;
pthread_once_t init_done=PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex=PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

void thread_init()
{
	pthread_key_create(&key,free);
}

char *getenv(const char *name)
{
	int i,len;char*envbuf;
	pthread_once(&init_done,thread_init);
	pthread_mutex_lock(&env_mutex);
	envbuf=(char*)pthread_getspecific(key);
	if(envbuf==NULL)
	{
		envbuf=(char*)malloc(ARG_MAX);
		if(envbuf==NULL)
		{
			pthread_mutex_unlock(&env_mutex);
			return NULL;
		}
		pthread_setspecific(key,envbuf);
	}
	len=strlen(name);
	for(i=0;environ[i]!=NULL;i++)
	{
		if((strncmp(name,environ[i],len)==0)&&(environ[i][len]=='='))
		{
			strcpy(envbuf,&environ[i][len+1]);
			pthread_mutex_unlock(&env_mutex);
			return envbuf;
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return NULL;
}