#include "common.h"
#include <limits.h>
#include <string.h>

static void envbuf[ARG_MAX];
extern char **environ;

char* getenv(const char*name)
{
	int i,len;
	len=strlen(name);
	for(i=0;environ[i]!=NULL;i++)
	{
		if((strncmp(name,environ[i],len)==0)&&(environ[i][len]=='='))
		{
			strcpy(envbuf,&environ[i][len+1]);
			return envbuf;
		}
	}
}
