include ../Make.defines

AR	= ar
LIBMISC	= libdb.a
OBJS   = db.o

RANLIB     = ranlib

PROGS = db process

all: process.c db.c
	${CC} ${CFLAGS} ${LDDIR} $^ ${LDLIBS} -o $@

db:	${OBJS}
	${AR} rv ${LIBMISC} $?
	$(MAKE) clean
	${RANLIB} ${LIBMISC}
	mv ${LIBMISC} ../include

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o *.idx *.dat all