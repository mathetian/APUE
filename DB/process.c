#include "common.h"
#include "db.h"

int main()
{
	DBHANDLE db;
	if((db=db_open("db4",O_RDWR|O_CREAT|O_TRUNC,FILE_MODE))==NULL) err_sys("db_open error\n");
	if(db_store(db,"Alpha","datal",DB_INSERT)!=0) err_quit("db_store error for Alpha\n");
	if(db_store(db,"beta","Data for beta",DB_INSERT)!=0) err_quit("db_store error for beta\n");
	if(db_store(db,"gamma","record3",DB_INSERT)!=0) err_quit("db_store error for gamma\n");
	db_close(db);
	exit(0);
}