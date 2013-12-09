#include "common.h"
#include <stropts.h>
//#include <sys/conf.h>

int main(int argc,char*argv[])
{
	int fd,i,nmods;
	struct str_list list;
	if(argc!=2) err_quit("usage\n");
	if((fd=open(argv[1],O_RDONLY)<0)) err_sys("can't open\n");
	if((isastream(fd))==0) err_quit("not a stream\n");
	/**Fetch the number**/
	if((nmods=ioctl(fd,I_LIST,(void*)0))<0) err_sys("I_LIST error for nmods\n");
	printf("#modules = %d\n",nmods);
	/**Allocate the storage for all module names**/
	if((list.sl_modlist=(struct str_mlist*)calloc(nmods,sizeof(struct str_mlist)))==NULL) err_sys("calloc error\n");
	list.sl_nmods=nmods;
	/**Fetch the module names**/
	if(ioctl(fd,I_LIST,&list)<0) err_sys("I_LIST error for list\n");
	/**Print the name**/
	for(i=0;i<=nmods;i++)
		printf(" %s: %s\n",(i==nmods)?"driver":"module",list.sl_modlist++->l_name);
}