#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
	fprintf(stderr,"EACCES: %s\n", strerror(EACCES));
	errno = ENOENT;
	return 0;
}