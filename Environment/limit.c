#include "common.h"

#if defined(BSD) || defined(MACOS)
#include <sys/time.h>
#define FMT "%10lld "
#else
#define FMT "%10ld "
#endif

#include <sys/resource.h>

#define doit(name) pr_limit(#name,name)

void pr_limit(const char*,int);
int main()
{
#ifdef RLIMIT_AS
	doit(RLIMIT_AS);
#endif
	doit(RLIMIT_CORE);
	doit(RLIMIT_CPU);
	doit(RLIMIT_DATA);
#ifdef RLIMIT_LOCKS
	doit(RLIMIT_LOCKS);
#endif
	doit(RLIMIT_STACK);
#ifdef RLIMIT_VMEM
	doit(RLIMIT_VMEM);
#endif
	exit(0);
}

void pr_limit(const char*name,int resource)
{
	struct rlimit limit;
	if(getrlimit(resource,&limit)<0) err_sys("getrlimit error");
	printf("%-14s ",name);
	if(limit.rlim_cur==RLIM_INFINITY) printf("(infinite)");
	else printf(FMT,limit.rlim_cur);
	if(limit.rlim_max==RLIM_INFINITY) printf("(infinite)");
	else printf(FMT,limit.rlim_max);
	putchar('\n');
}