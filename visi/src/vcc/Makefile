
SHELL   = /bin/sh

#       visi
#
#       Сборка препроцессора описаний страниц
#       для компилятора СИ и для lbp
#
#  ПРИМЕЧАНИЕ:
#       Из одного и того же текста получается две программы.

CC      = cc

DEST    = ../../bin
CFLAGS  = -I../../include

CO      = co
COFLAGS =

MAKEFILE = Makefile

#
#       СБОРКА ПРИКЛАДНОЙ ПРОГРАММЫ БЕЗ УСТАНОВКИ
#       БИБЛИОТЕК LINLIB НА СТАНДАРТНЫЕ МЕСТА
#VISI          = ../../../visi
VISI          = ../..
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

LIBS          = \
		$(VLIB)/libline.a

all:    vlbp    vcc

vcc:    vcc.c vgen.h $(LIBS)
	$(CC) -o vcc -DVCC $(CFLAGS) vcc.c $(LIBS)

vlbp:   vcc.c vgen.h $(LIBS)
	$(CC) -o vlbp -DVLBP $(CFLAGS) vcc.c $(LIBS)

install: vcc vlbp
	-strip vcc vlbp
	mv vcc vlbp $(DEST)

clean:; -rm -f *.o

depend:;        mkmf -f $(MAKEFILE) DEST=$(DEST)

$(DEST):;       mkdir $(DEST)

co:     .co

.co:
	$(CO) $(COFLAGS) vcc.c vgen.h
	touch .co

.DEFAULT:
	$(CO) $(COFLAGS) $@
###
vcc.o: /usr/include/ediag.h /usr/include/stdio.h /usr/include/ctype.h \
	../../include/line.h ../../include/linebp.h vgen.h
