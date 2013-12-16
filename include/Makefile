include ../Make.defines

AR	= ar
LIBMISC	= libcommon.a
OBJS   = pipe.o print.o other.o daemon.o file.o

RANLIB     = ranlib

all:	${OBJS}
	${AR} rv ${LIBMISC} $?
	$(MAKE) clean
	${RANLIB} ${LIBMISC}

clean:
	rm -f *.o

# all:
# 	for i in $(DIRS); do \
# 		(cd $$i && $(MAKE)) || exit 1; \
# 	done

# clean:
# 	for i in $(DIRS); do \
# 		(cd $$i && $(MAKE) clean) || exit 1; \
# 	done