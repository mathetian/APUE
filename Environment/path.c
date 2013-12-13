#include "common.h"

void pr_sysconf(const char*, int);
void pr_pathconf(const char*, const char*, int);

int main(int argc,char*argv[])
{
	if(argc!=2) error_quit("usage error\n");
#ifdef ARG_MAX
	printf("ARG_MAX %d:\n",ARG_MAX+0);
#else
	printf("No ARG_MAX\n");
#endif
#ifdef MAX_CANON
	printf("MAX_CANON %d\n",MAX_CANON+0);
#else
	printf("no symbol for MAX_CANON\n");
#endif
#ifdef _PC_MAX_CANON
	pr_pathconf("MAX_CANON =", argv[1],_PC_MAX_CANON);
#else
	printf("No symbol for _PC_MAX_CANON\n");
#endif
	exit(0);
}

void pr_sysconf(const char*msg, int name)
{
	long val;fputs(msg,stdout);
	errno=0;if((val=sysconf(name))<0)
	{
		if(errno!=0)
		{
			if(errno==EINVAL) fputs("not supported\n",stdout);
			else fputs("sysconf error\n",stderr);
		}
		else fputs("no limit\n",stdout);
	}
	else printf(" %ld\n",val);
}

void pr_pathconf(const char*msg, const char*path, int name)
{
	long val;fputs(msg,stdout);errno=0;
	if((val=pathconf(path,name))<0)
	{
		printf("what's rong\n");
	}
	else
		printf(" %ld\n",val);
}