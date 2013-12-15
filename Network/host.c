#include "common.h"
#include <netdb.h>
#include <arpa/inet.h>
#if defined(BSD) || defined(MACOS)
#include <sys/socket.h>
#include <netinet/in.h>
#endif

void print_family(struct addrinfo*aip){}
void print_type(struct addrinfo*aip){}
void print_protocol(struct addrinfo*aip){}
void print_flags(struct addrinfo*aip){}

int main(int argc,char*argv[])
{
	struct addrinfo *alist,*aip;struct addrinfo hint;
	struct sockaddr_in *sinp;const char *addr;int err;
	char abuf[INET_ADDRSTRLEN];
	if(argc!=3) err_quit("usage error\n");
	hint.ai_flags=0;hint.ai_family=0;hint.ai_socktype=0;hint.ai_protocol=0;
	hint.ai_addrlen=0;hint.ai_canonname=NULL;hint.ai_addr=NULL;hint.ai_next=NULL;
	if((err=getaddrinfo(argv[1],argv[2],&hint,&alist))!=0) err_quit("getaddrinfo error\n");
	for(aip=alist;aip!=NULL;aip=aip->ai_next)
	{
		print_flags(aip);print_family(aip);
		print_type(aip);print_protocol(aip);
		printf("\n\thost %s",aip->ai_canonname?aip->ai_canonname:"-");
		if(aip->ai_family==AF_INET)
		{
			sinp=(struct sockaddr_in *)aip->ai_addr;
			addr=inet_ntop(AF_INET,&sinp->sin_addr,abuf,INET_ADDRSTRLEN);
			printf(" address %s",addr?addr:"unknown");
			printf(" port %d",ntohs(sinp->sin_port));
		}
		printf("\n");
	}
	exit(0);
}