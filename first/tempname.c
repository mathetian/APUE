#include "common.h"

int main(int argc,char*argv[])
{
	if(argc!=3) err_quit("Usage\n");
	printf("%s\n",tempnam(argv[1],argv[2]));
	sleep(1000000);
	return 0;
}