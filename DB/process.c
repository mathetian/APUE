#include "common.h"
#include "db.h"

int main()
{
	DBHANDLE db;char key[15];char*value;
	if((db=db_open("db4",O_RDWR|O_CREAT|O_TRUNC,FILE_MODE))==NULL) err_sys("db_open error\n");
	printf("open succeed\n");
	if(db_store(db,"Alpha","datal",DB_INSERT)!=0) err_quit("db_store error for Alpha\n");
	if(db_store(db,"beta","Data for beta",DB_INSERT)!=0) err_quit("db_store error for beta\n");
	if(db_store(db,"gamma","record3",DB_INSERT)!=0) err_quit("db_store error for gamma\n");
	if(db_store(db,"beta","test",DB_REPLACE)!=0) err_quit("db_store error for beta2\n");
	db_delete(db,"gamma");db_rewind(db);memset(key,0,sizeof(key));
	while((value=db_nextrec(db,key))!=NULL) printf("%s %s\n",key,value);
	db_close(db);exit(0);
}