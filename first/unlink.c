#include "common.h"

int main()
{
	if(open("tempfile",O_RDWR)<0)
		err_sys("open error\n");
	if(unlink("tempfile")<0)
		err_sys("unlink failure\n");
	printf("file unlinked\n");
	sleep(15000);
	printf("done\n");
	exit(0);
}