
SHELL   = /bin/sh

#
# ИСПРАВЛЕНО ПО ДИСТРИБУТИВУ ДЕМОС 2.0/#5 (КИАЭ, МАРТ 1987).
# ДЕМОС 2.0/#8
#
# ВНЕСЕНЫ ПРАВКИ ДЛЯ linlib:
#  НАДО ОБХОДИТЬ ВЫДАЧУ КОДОВ ^n ^o ^j
#

CFLAGS  = -O -DCM_N -DCM_GT -DCM_B -DCM_D

DEST    = ../../lib

HDRS    =

EXTHDRS =

SRCS	= test4.c

OBJS	= test4.o


all: test4

install:

clean:
	-rm -f *.o *.a test4

.c.o:
	$(CC) -c $(CFLAGS) $*.c
#        mv $*.o tmp.o
#        ld -r -x -o $*.o tmp.o

test4:	test4.o
	$(CC) -o $@ -l termcap test4.o

VGRIND=	csh /usr/ucb/vgrind
vgrind:
	cp /dev/null index
	${VGRIND} -h "Termcap library" termcap.c tputs.c tgoto.c
	${VGRIND} -h "Termcap library" -x index

co:;    : "Нет архива RCS -- не используется для termnew"

depend:; : "Нет необходимости настраивать Makefile"
###
