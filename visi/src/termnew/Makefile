
SHELL   = /bin/sh

#
# יףנעבקלומן נן היףפעיגץפיקץ הוםןף 2.0/#5 (כיבü, םבעפ 1987).
# הוםןף 2.0/#8
#
# קמוףומש נעבקכי הלס linlib:
#  מבהן ןגטןהיפר קשהב‏ץ כןהןק ^n ^o ^j
#

CFLAGS  = -O -DCM_N -DCM_GT -DCM_B -DCM_D

DEST    = ../../lib

HDRS    =

EXTHDRS =


all: libtermnew.a

install: libtermnew.a
	mv libtermnew.a ${DEST}/libtermnew.a
	-ranlib ${DEST}/libtermnew.a

libtermnew.a: termcap.o tgoto.o tputs.o
	ar cr libtermnew.a termcap.o tgoto.o tputs.o

clean:
	-rm -f *.o

.c.o:
	$(CC) -c $(CFLAGS) $*.c
#        mv $*.o tmp.o
#        ld -r -x -o $*.o tmp.o

VGRIND=	csh /usr/ucb/vgrind
vgrind:
	cp /dev/null index
	${VGRIND} -h "Termcap library" termcap.c tputs.c tgoto.c
	${VGRIND} -h "Termcap library" -x index

co:;    : "מֵװ ֱָֹׂ׳ֱ RCS -- ־ֵ ֹ׃׀ּֿ״Úױֵװ׃ׁ ִּׁ termnew"

depend:; : "מֵװ ־ֲִֵָֹֽֿֿֿ׃װֹ ־ֱ׃װֱֹׂ׳ֱװ״ Makefile"
###
