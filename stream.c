#include "common.h"

void pr_stdio(const char*,FILE*);

int main()
{
	FILE*fp;
	fputs("enter any character\n", stdout);
	if(getchar()==EOF) err_sys("getchar error\n");
	fputs("one line to standard error\n",stderr);
	pr_stdio("stdin",stdin);
	pr_stdio("stdout",stdout);
	pr_stdio("stderr",stderr);
	if((fp=fopen("/home/mathewes/dataCenter/APUEReadingNote/file.c","r"))==NULL) err_sys("fopen error\n");
	if(getc(fp)==EOF) err_sys("getc error\n");	
	pr_stdio("/home/mathewes/dataCenter/APUEReadingNote/file.c",fp);
	exit(0);
}

void pr_stdio(const char*name, FILE*fp)
{
	printf("stream = %s, ",name);
	if(fp->_IO_file_flags & _IO_UNBUFFERED)
		printf("unbuffered");
	else if(fp->_IO_file_flags & _IO_LINE_BUF)
		printf("line buffer");
	else printf("fully buffered");
	int size=fp->_IO_buf_end-fp->_IO_buf_base;
	printf(", buffer size = %d\n",size);
}