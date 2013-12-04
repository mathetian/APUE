#include "common.h"

int main()
{
#ifdef PATH_MAX
	int pathmax = PATH_MAX;
#else
	int pathmax = 0;
#endif
printf("PATH_MAX: %d\n",PATH_MAX);
exit(0);
}