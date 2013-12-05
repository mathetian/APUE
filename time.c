#include "common.h"
#include <sys/times.h>

void do_cmd(const char*);
void pr_times(clock_t, struct tms*,struct tms*);

int main(int argc,char*argv[])
{
	int i;setbuf(stdout,NULL);
	for(i=1;i<argc;i++) do_cmd(argv[i]);
	exit(0);
}

void do_cmd(const char*cmd)
{
	struct tms tmstart, tmsend;
	clock_t start, end;int status;
	printf("\ncommand: %s\n",cmd);
	if((start=times(&tmstart))==-1) err_sys("times error\n");
	if((status=system(cmd))<0) err_sys("system() error\n");
	if((end=times(&tmsend))==-1) err_sys("times error\n");
	pr_times(end-start,&tmstart,&tmsend);
}

void pr_times(clock_t real, struct tms *tmstart,struct tms *tmsend)
{
	static long clktck=0;
	if(clktck==0) if((clktck=sysconf(_SC_CLK_TCK))<0) err_sys("sysconf error\n");
	printf(" real: %7.2f\n",real/(double)clktck);
	printf(" user: %7.2f\n",(tmsend->tms_utime-tmstart->tms_utime)/(double)clktck);
	printf("  sys: %7.2f\n",(tmsend->tms_stime-tmstart->tms_stime)/(double)clktck);
	printf("child user: %7.2f\n",(tmsend->tms_cutime-tmstart->tms_cutime)/(double)clktck);
	printf("child sys: %7.2f\n",(tmsend->tms_cstime-tmstart->tms_cstime)/(double)clktck);
}