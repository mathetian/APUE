#include "common.h"

int main()
{
	umask(0);
	if(creat("foo",FILE_MODE)<0)
		err_sys("create foo error\n");
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if(creat("bar",FILE_MODE)<0)
		err_sys("create bar error\n");
	exit(0);
}