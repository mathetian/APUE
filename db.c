#include "common.h"
#include "db.h"

#define IDXLEN_SZ  4
#define SEP 	  ':'
#define SPACE 	  ' '
#define NEWLINE   '\n'

#define PTR_SZ	   6
#define PTR_MAX    999999
#define NHASH_DEF  137
#define FREE_OFF   0
#define HASH_OFF   PTR_SZ

typedef unsigned long DBHASH;
typedef unsigned long COUNT;

typedef struct{
	int idxfd;
	int datfd;
	char *idxbuf;
	char *datbuf;
	char *name;
	off_t idxoff;
	size_t idxlen;
	off_t datoff;
	size_t datlen;
	off_t ptrval;
	off_t ptroff;
	off_t chainoff;
	off_t hashoff;
	DBHASH nhash;
	COUNT cnt_delok;
	COUNT cnt_delerr;
	COUNT cnt_fetchok;
	COUNT cnt_fetcherr;
	COUNT cnt_nextrec;
	COUNT cnt_stor1;
	COUNT cnt_stor2;
	COUNT cnt_stor3;
	COUNT cnt_stor4;
	COUNT cnt_storerr;
} DB;

static DB *_db_alloc(int);
static void _db_dodelete(DB*);
static int _db_find_and_lock(DB*,const char*,int);
static int _db_findfree(DB*,int,int);
static void _db_free(DB*);
static DBHASH _db_hash(DB*,const char*);
static char *_db_readdat(DB*);
static off_t _db_readidx(DB*,off_t);
static off_t _db_readptr(DB*,off_t);
static void _db_writedat(DB*,const char*,off_t,int);
static void _db_writeidx(DB*,const char*,off_t,int,off_t);
static void _db_writeptr(DB*,off_t,off_t);


DBHANDLE db_open(const char*pathname,int oflag,...)
{
	DB*db;
	int len,mode;
	size_t i;
	char asciiptr[PTR_SZ+1], hash[(NHASH_DEF+1)*PTR_SZ+2];
	struct stat statbuff;
	/**Allocate a DB structure and the buffers it needs**/
	len=strlen(pathname);
	if((db=_db_alloc(len))==NULL) err_sys("db_open: _db_alloc error for DB\n");
	db->nhash=NHASH_DEF;db->hashoff=HASH_OFF;
	strcpy(db->name,pathname);strcpy(db->name+len,".idx");
	if(oflag&O_CREAT) 
	{
		va_list ap;
		va_start(ap,oflag);
		mode=va_arg(ap,int);
		va_end(ap);
		/**Open index file and data file**/
		db->idxfd=open(db->name,oflag,mode);
		strcpy(db->name+len,".dat");
		db->datfd=open(db->name,oflag,mode);
	}
	else
	{
		/**Open index file and data file**/
		db->idxfd=open(db->name,oflag);
		strcpy(db->name+len,".dat");
		db->datfd=open(db->name,oflag);
	}
	if(db->idxfd<0||db->datfd<0)
	{
		_db_free(db);return NULL;
	}
	if((oflag&(O_CREAT|O_TRUNC))==(O_CREAT|O_TRUNC))
	{
		if(writew_lock(db->idxfd,0,SEEK_SET,0)<0) err_sys("writew_lock error\n");
		if(fstat(db->idxfd,&statbuff)<0) err_sys("fstat error\n");
		if(statbuff.st_size==0)
		{
			sprintf(asciiptr,"%*d",PTR_SZ,0);hash[0]=0;
			for(i=0;i<NHASH_DEF;i++) strcat(hash,asciiptr);
			strcat(hash,"\n");i=strlen(hash);
			if(write(db->idxfd,hash,1)!=i)
				err_sys("db_open: index file init write error\n");
		}
		if(un_lock(db->idxfd,0,SEEK_SET,0)<0) err_sys("un_lock error\n");
	}
	db_rewind(db);
	return db;		
}

static DB*_db_alloc(int namelen)
{
	DB*db;
	if((db=(DB*)calloc(1,sizeof(DB)))==NULL) err_sys("_db_alloc eror\n");
	db->idxfd=db->datfd=-1;
	/**Allocate room for the name**/
	if((db->name=(char*)malloc(namelen+5))==NULL) err_sys("malloc error\n");
	/**Allocate an index buffer**/
	if((db->idxbuf=(char*)malloc(IDXLEN_MAX+2))==NULL)
		err_sys("malloc error\n");
	if((db->datbuf=(char*)malloc(IDXLEN_MAX+2))==NULL)
		err_sys("malloc error\n");
	return db;
}

void db_close(DBHANDLE h)
{
	_db_free((DB*)h);
}

static void _db_free(DB*db)
{
	if(db->idxfd>=0) close(db->idxfd);
	if(db->datfd>=0) close(db->datfd);
	if(db->idxbuf!=NULL) free(db->idxbuf);
	if(db->datbuf!=NULL) free(db->datbuf);
	if(db->name!=NULL) free(db->name);
	free(db);
}

char*db_fetch(DBHANDLE h,const char*key)
{
	DB*db=(DB*)h;
	char*ptr;
	if(_db_find_and_lock(db,key,0)<0) 
	{
		ptr=NULL;db->cnt_fetcherr++;
	}
	else
	{
		ptr=_db_readdat(db);db->cnt_fetchok++;
	}
	if(un_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0) 
		err_sys("un_lock error\n");
	return ptr;
}

static int _db_find_and_lock(DB*db,const char*key,int writelock)
{
	off_t offset; off_t nextoffset;
	db->chainoff=(_db_hash(db,key)*PTR_SZ)+db->hashoff;
	db->ptroff=db->chainoff;
	if(writelock)
	{
		if(writew_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0)
			err_sys("writew_lock error\n");
	}
	else
	{
		if(readw_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0)
			err_sys("readw_lock error\n");
	}
	offset=_db_readptr(db,db->ptroff);
	while(offset!=0)
	{
		nextoffset=_db_readidx(db,offset);
		if(strcmp(db->idxbuf,key)==0) break;
		db->ptroff=offset;
		offset=nextoffset;
	}
	return offset==0?-1:0;
}

static DBHASH _db_hash(DB*db,const char*key)
{
	DBHASH hval=0;
	char c;int i;
	for(i=0;(c=*key++)!=0;i++) hval+=c*i;
	return hval%db->nhash;
}

static off_t _db_readptr(DB*db,off_t offset)
{
	char asciiptr[PTR_SZ+1];
	if(lseek(db->idxfd,offset,SEEK_SET)==-1) err_sys("lseek error\n");
	if(read(db->idxfd,asciiptr,PTR_SZ)!=PTR_SZ) err_sys("read error\n");
	asciiptr[PTR_SZ]=0;
	return atol(asciiptr);
}

static off_t _db_readidx(DB*db,off_t offset)
{
	ssize_t i;
	char *ptr1,*ptr2;
	char asciiptr[PTR_SZ+1],asciilen[IDXLEN_SZ+1];
	struct iovec iov[2];
	if((db->idxoff=lseek(db->idxfd,offset,offset==0?SEEK_CUR:SEEK_SET))==-1)
		err_sys("lseek error\n");
	iov[0].iov_base=asciiptr;iov[0].iov_len=PTR_SZ;
	iov[1].iov_base=asciilen;iov[1].iov_len=IDXLEN_SZ;
	if((i=readv(db->idxfd,&iov[0],2))!=PTR_SZ+IDXLEN_SZ)
	{
		if(i==0&&offset==0) return -1;
		err_sys("readv error\n");
	}
	asciiptr[PTR_SZ]=0;
	db->ptrval=atol(asciiptr);
	asciilen[IDXLEN_SZ]=0;
	if((db->idxlen=atoi(asciilen))<IDXLEN_MIN||db->idxlen>IDXLEN_MAX)
		err_sys("invalid length\n");
	/**Now read the actual index record. We read it into the key buffer**/
	if((i=read(db->idxfd,db->idxbuf,db->idxlen))!=db->idxlen)
		err_sys("read error of index record\n");
	if(db->idxbuf[db->idxlen-1]!=NEWLINE)
		err_sys("missing line\n");
	db->idxbuf[db->idxlen-1]=0;
	/**Find the seperators in the index record**/
	if((ptr1=strchr(db->idxbuf,SEP))==NULL)
		err_sys("missing first seperator\n");
	*ptr1++=0;
	if((ptr2=strchr(ptr1,SEP))==NULL)
		err_sys("missing second seperator\n");
	if(strchr(ptr2,SEP)!=NULL) err_sys("too many seperator\n");
	if((db->datoff=atol(ptr1))<0) err_sys("starting offset < 0\n");
	if((db->datlen=atol(ptr2))<0) err_sys("invalid length\n");
	return db->ptrval;
}

static char* _db_readdat(DB*db)
{
	if(lseek(db->datfd,db->datoff,SEEK_SET)==-1) 
		err_sys("_db_readdat: lseek error\n");
	if(read(db->datfd,db->datbuf,db->datlen)!=db->datlen)
		err_sys("_db_readdat: read error\n");
	if(db->datbuf[db->datlen-1]!=NEWLINE)
		err_sys("_db_readdat: missing NEWLINE");
	db->datbuf[db->datlen-1]=0;
	return db->datbuf;
}

int db_delete(DBHANDLE h,const char *key)
{
	DB*db=(DB*)h;int rc=0;
	if(_db_find_and_lock(db,key,1)==0)
	{
		_db_dodelete(db);db->cnt_delok++;
	}
	else
	{
		rc=-1;db->cnt_delerr++;
	}
	if(un_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0)
		err_sys("db_delete: un_lock error\n");
	return rc;
}

static void _db_dodelete(DB*db)
{
	int i;char*ptr;
	off_t freeptr,saveptr;
	/**Set data buffer and key to all blanks**/
	for(ptr=db->datbuf,i=0;i<db->datlen-1;i++) *ptr++=SPACE;
	*ptr=0;ptr=db->idxbuf;
	while(*ptr) *ptr++=SPACE;
	if(writew_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0)
		err_sys("_db_dodelete: writew_lock eror\n");
	/**Write the data record with all blanks**/
	_db_writedat(db,db->datbuf,db->datoff,SEEK_SET);
	/**Read the free list pointer**/
	freeptr=_db_readptr(db,FREE_OFF);
	/**Save the contents of index record**/
	saveptr=db->ptrval;
	/**Rewrite the index record**/
	_db_writeidx(db,db->idxbuf,db->idxoff,SEEK_SET,freeptr);
	_db_writeptr(db,FREE_OFF,db->idxoff);
	/**Rewrite the chain ptr**/
	_db_writeptr(db,db->ptroff,saveptr);
	if(un_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0)
		err_sys("_db_dodelete: un_lock error\n");
}

static void _db_writedat(DB*db,const char*data,off_t offset,int whence)
{
	struct iovec iov[2];static char newline=NEWLINE;
	/**If append, need lock. Otherwise, don't need**/
	if(whence==SEEK_END)
	{
		if(writew_lock(db->datfd,0,SEEK_SET,0)<0)
			err_sys("_db_writedat: writew_lock error\n");
	}
	if((db->datoff=lseek(db->datfd,offset,whence))==-1)
		err_sys("_db_writedat: lseek error\n");
	db->datlen=strlen(data)+1;
	iov[0].iov_base=(char*)data;iov[0].iov_len=db->datlen-1;
	iov[1].iov_base=&newline;iov[1].iov_len=1;
	if(writev(db->datfd,&iov[0],2)!=db->datlen)
		err_sys("_db_writedat: writev error of data record\n");
	if(whence==SEEK_END)
	{
		if(un_lock(db->datfd,0,SEEK_SET,0)<0)
			err_sys("_db_writedat: un_lock error\n");
	}
}

static void _db_writeidx(DB*db,const char*key,off_t offset,int whence,off_t ptrval)
{
	struct iovec iov[2];char asciiptrlen[PTR_SZ+IDXLEN_SZ+1];
	int len;const char*fmt;
	if((db->ptrval=ptrval)<0||ptrval>PTR_MAX)
		err_quit("_db_writeidx: invalid ptr\n");
	if(sizeof(off_t)==sizeof(long long)) fmt="%s%c%lld%c%d\n";
	else fmt="%s%c%ld%c%d\n";
	sprintf(db->idxbuf,fmt,key,SEP,db->datoff,SEP,db->datlen);
	if((len=strlen(db->idxbuf))<IDXLEN_MIN||len>IDXLEN_MAX)
		err_sys("_db_writeidx: invalid length\n");
	sprintf(asciiptrlen,"%*ld%*d",PTR_SZ,ptrval,IDXLEN_SZ,len);
	/**lock or not**/
	if(whence==SEEK_END)
	{
		if(writew_lock(db->idxfd,((db->nhash+1)*PTR_SZ)+1,SEEK_SET,0)<0)
			err_sys("_db_writeidx: writew_lock error\n");
	}
	/**Position the index file and record the offset**/
	if((db->idxoff=lseek(db->idxfd,offset,whence))==-1) 
		err_sys("_db_writeidx: lseek error\n");
	iov[0].iov_base=asciiptrlen;iov[0].iov_len=PTR_SZ+IDXLEN_SZ;
	iov[1].iov_base=db->idxbuf;iov[1].iov_len=len;
	if(writev(db->idxfd,&iov[0],2)!=PTR_SZ+IDXLEN_SZ+len)
		err_sys("_db_writeidx: writev error\n");
	if(whence==SEEK_END)
	{
		if(un_lock(db->idxfd,((db->nhash+1)*PTR_SZ)+1,SEEK_SET,0)<0)
			err_sys("_db_writeidx: un_lock error\n");
	}
}

/**Write a chain ptr field somewhere in the index file**/
static void _db_writeptr(DB*db,off_t offset,off_t ptrval)
{
	char asciiptr[PTR_SZ+1];
	if(ptrval<0||ptrval>PTR_MAX)
		err_quit("_db_writeptr: invalid paramter\n");
	sprintf(asciiptr,"%*ld",PTR_SZ,ptrval);
	if(lseek(db->idxfd,offset,SEEK_SET)==-1)
		err_sys("_db_writeptr: lseek error to ptr field\n");
	if(write(db->idxfd,asciiptr,PTR_SZ)!=PTR_SZ)
		err_sys("_db_writeptr: write error\n");
}

int db_store(DBHANDLE h,const char*key,const char*data,int flag)
{
	DB*db=(DB*)h;int rc,keylen,datlen;
	off_t ptrval;
	if(flag!=DB_INSERT&&flag!=DB_REPLACE&&flag!=DB_STORE)
	{
		errno=EINVAL;return -1;
	}
	keylen=strlen(key);datlen=strlen(data)+1;
	if(datlen<DATLEN_MIN||datlen>DATLEN_MAX)
		err_sys("db_store: invalid length\n");
	if(_db_find_and_lock(db,key,1)<0)
	{
		if(flag==DB_REPLACE)
		{
			rc=-1;db->cnt_storerr++;
			errno=ENOENT;goto doreturn;
		}
		ptrval=_db_readptr(db,db->chainoff);
		if(_db_findfree(db,keylen,datlen)<0)
		{
			_db_writedat(db,data,0,SEEK_END);
			_db_writeidx(db,key,0,SEEK_END,ptrval);
		}
		else
		{
			/**Use the empty record**/
			_db_writedat(db,data,db->datoff,SEEK_SET);
			_db_writeidx(db,key,db->idxoff,SEEK_SET,ptrval);
			_db_writeptr(db,db->chainoff,db->idxoff);
			db->cnt_stor2++;
		}
	}
	else
	{
		/**Record found**/
		if(flag==DB_INSERT)
		{
			rc=1;db->cnt_storerr++;
			goto doreturn;
		}
		/**Replace it**/
		if(datlen!=db->datlen)
		{
			_db_dodelete(db);
			ptrval=_db_readptr(db,db->chainoff);
			/**Append new index and data records to end of files**/
			_db_writedat(db,data,0,SEEK_END);
			_db_writeidx(db,key,0,SEEK_END,ptrval);
			_db_writeptr(db,db->chainoff,db->idxoff);
		}
		else
		{
			/**Same size data, just replace data record**/
			_db_writedat(db,data,db->datoff,SEEK_SET);
			db->cnt_stor4++;
		}
	}
	rc=0;
doreturn:
	if(un_lock(db->idxfd,db->chainoff,SEEK_SET,1)<0)
		err_sys("db_store: un_lock error\n");
	return rc;
}

static int _db_findfree(DB*db,int keylen,int datlen)
{
	int rc;
	off_t offset,nextoffset,saveoffset;
	/**Lock the free list**/
	if(writew_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0)
		err_sys("_db_findfree: writew_lock error\n");
	/**Read the free list pointer**/
	saveoffset=FREE_OFF;
	offset=_db_readptr(db,saveoffset);
	while(offset!=0)
	{
		nextoffset=_db_readidx(db,offset);
		if(strlen(db->idxbuf)==keylen&&db->datlen==datlen)
			break;
		saveoffset=offset;
		offset=nextoffset;
	}
	if(offset==0) rc=-1;
	else
	{
		/**Found a free record**/
		_db_writeptr(db,saveoffset,db->ptrval);
		rc=0;
	}
	/**Unlock the free list**/
	if(un_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0)
		err_sys("_db_findfree: un_lock error\n");
	return rc;
}

/**Rewind the index file for db_nextrec**/
void db_rewind(DBHANDLE h)
{
	DB*db=(DB*)h;off_t offset;
	offset=(db->nhash+1)*PTR_SZ;
	if((db->idxoff=lseek(db->idxfd,offset+1,SEEK_SET))==-1)
		err_sys("db_rewind: lseek error\n");
}

/**Return the next sequential record**/
char* db_nextrec(DBHANDLE h,char*key)
{
	DB*db=(DB*)h;char c;char*ptr;
	if(readw_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0)
		err_sys("db_nextrec: readw error\n");
	do{
		/**Read**/
		if(_db_readidx(db,0)<0)
		{
			ptr=NULL;goto doreturn;
		}
		/**Check if key is all blank**/
		ptr=db->idxbuf;
		while((c=*ptr++)!=0&&c==SPACE);
	}while(c==0);
doreturn:
	if(un_lock(db->idxfd,FREE_OFF,SEEK_SET,1)<0)
		err_sys("db_nextrec: un_lock error\n");
	return ptr;
}