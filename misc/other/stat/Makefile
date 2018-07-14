SHELL=/bin/sh

CFLAGS = -DVERBOSE -C

all:	statm stat

statm:  statm.c
	cc ${CFLAGS} -o statm statm.c

stat:   stat.c
	cc ${CFLAGS} -o stat stat.c

install: stat
	mv ./stat ${HOME}/bin/stat

clean:
	rm -f *.o *.b *~ stat statm .,resv*
