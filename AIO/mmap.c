#include "common.h"
#include <sys/mman.h>

int main(int argc,char*argv[])
{
	int fdin,fdout;void *src,*dst;
	struct stat statbuf;
	if(argc!=3) err_quit("usage\n");
	if((fdin=open(argv[1],O_RDONLY,FILE_MODE))<0) err_sys("can't create for writting\n");
	if((fdout=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,FILE_MODE)))
	if(fstat(fdin,&statbuf)<0) err_sys("fstat error\n");
	if(lseek(fdout,statbuf.st_size-1,SEEK_SET)==-1) err_sys("lseek error\n");
	if(write(fdout,"",1)!=1) err_sys("write error\n");
	if((src=mmap(0,statbuf.st_size,PROT_READ,MAP_SHARED,fdin,0))==MAP_FAILED) err_sys("mmap error for input\n");
	if((dst=mmap(0,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fdout,0))==MAP_FAILED) err_sys("mmap error for output\n");
	memcpy(dst,src,statbuf.st_size);
	exit(0);
}