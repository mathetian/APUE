#include "common.h"
#include <dirent.h>
#include <limits.h>

typedef int MyFunc(const char*, const struct stat *, int);

MyFunc myfunc;
int myftw(char*, MyFunc *);
int dopath(MyFunc *);
long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc,char*argv[])
{
	int ret;
	if(argc!=2) err_quit("Usage error\n");
	ret=myftw(argv[1],myfunc);
	ntot=nreg+ndir+nblk+nchr+nfifo+nslink+nsock;
	if(ntot==0) ntot=1;
	printf("regular files = %7ld, %5.2f %%\n",nreg,nreg*100.0/ntot);
	printf("dir files     = %7ld, %5.2f %%\n",ndir,ndir*100.0/ntot);
	printf("block         = %7ld, %5.2f %%\n",nblk,nblk*100.0/ntot);
	printf("char          = %7ld, %5.2f %%\n",nchr,nchr*100.0/ntot);
	printf("FIFOs         = %7ld, %5.2f %%\n",nfifo,nfifo*100.0/ntot);
	exit(0);
}

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

char*fullpath;

int myftw(char*pathname,MyFunc *func)
{
	int len;fullpath=path_alloc(&len);
	strncpy(fullpath,pathname,len);
	fullpath[len-1]=0;
	return dopath(func);
}

int dopath(MyFunc*func)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;int ret;char *ptr;
	if(lstat(fullpath,&statbuf)<0) return func(fullpath,&statbuf,FTW_NS);
	if(S_ISDIR(statbuf.st_mode)==0) return func(fullpath,&statbuf,FTW_F);
	if((ret=func(fullpath,&statbuf,FTW_D))!=0) return ret;
	ptr=fullpath+strlen(fullpath);*ptr++='/';*ptr=0;
	if((dp=opendir(fullpath))==NULL) return func(fullpath,&statbuf,FTW_DNR);
	while((dirp=readdir(dp))!=NULL)
	{
		if(strcmp(dirp->d_name,".")==0||strcmp(dirp->d_name,"..")==0) continue;
		strcpy(ptr,dirp->d_name);
		if((ret=dopath(func))!=0) break;
	}
	ptr[-1]=0;if(closedir(dp)<0) err_sys("close dp error\n");
	return ret;
}

int myfunc(const char*pathname, const struct stat *statptr, int type)
{
	switch(type)
	{
	case FTW_F:
		switch(statptr->st_mode & S_IFMT)
		{
		case S_IFREG: nreg++; break;
		case S_IFBLK: nblk++; break;
		case S_IFCHR: nchr++; break;
		case S_IFLNK: nslink++; break;
		case S_IFSOCK: nsock++; break;
		case S_IFDIR: err_sys("if dir?\n");
		}
		break;
	case FTW_D: ndir++; break;
	case FTW_DNR: err_sys("can't read directory\n");break;
	case FTW_NS: err_sys("stat error\n");break;
	default: err_sys("unknown type\n");break;
	}
	return 0;
}
