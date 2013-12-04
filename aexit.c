#include "common.h"

void my_exit1();
void my_exit2();

int main()
{
	if(atexit(my_exit1)!=0) err_sys("can't register exit1\n");
	if(atexit(my_exit2)!=0) err_sys("can't register exit2\n");
	if(atexit(my_exit2)!=0) err_sys("can't register exit2\n");
	printf("main is done\n");
	return 15;
}

void my_exit1()
{
	printf("first handled\n");
}

void my_exit2()
{
	printf("second handled\n");
}