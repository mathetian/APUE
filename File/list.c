#include <dirent.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	DIR*dp;struct dirent*dirp;
	if ((dp = opendir(".")) == NULL)
	{
		printf("error open\n");
		return 0;
	}
	while ((dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);
	closedir(dp);
	return 0;
}