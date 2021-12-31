
SHELL         = /bin/sh

include		../conf/Makefile
include		../conf.cfg

##
##      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
##     (C) LINLIB  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
##      +----------+    ВИДЕОТЕРМИНАЛОВ
##
##      $Header$
##      $Log$
##

#
#       СБОРКА ПРИКЛАДНОЙ ПРОГРАММЫ БЕЗ УСТАНОВКИ
#       БИБЛИОТЕК LINLIB НА СТАНДАРТНЫЕ МЕСТА
#
VISI          = ../visi
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

PROGRAM       = vtest
EXECMODE      =  711
#DEST          = /usr/local
DESTDIR       =
DEST          = /usr

DESTBIN       = $(DESTDIR)$(DEST)/bin
DESTLIB       = $(DESTDIR)$(DEST)/lib

CFLAGS        = -I$(VISI)/include \
		-D'VEXDIR="$(DESTLIB)/$(PROGRAM)"'

#CC            = cc -O
#LINKER        = cc

LDFLAGS       =
#STDLIBS       = -ltermcap
STDLIBS       = $(TERMLIB)
LIBS          = \
		$(VLIB)/liblinebp.a \
		$(VLIB)/liblinusr.a \
		$(VLIB)/libline.a \
#		$(VLIB)/libtermnew.a

#VCC           = $(VBIN)/vcc
#VLBP          = $(VBIN)/vlbp
#LBP           = $(VBIN)/lbp
VCC           = vcc
VLBP          = vlbp
LBP           = lbp
MAKEFILE      = Makefile

EXTHDRS	      = $(VISI)/include/line.h \
		$(VISI)/include/line0.h \
		$(VISI)/include/linebp.h \
		$(VISI)/include/lineva.h \
		/usr/include/stdio.h

HDRS	      = 

IPAGES	      = pmainv.i

#VPAGES        = pmainv.cv

SRCS          = pmainv.cv \
		pmainv.c \
		pmain.c

OBJS	      = pmain.o \
		pmainv.o

SUFFIX        = .cv:sC

.SUFFIXES : .out .o .c .cm .f .e .r .y .yr .ye .l .s .cl .p .q \
	    .i .h .cv .la .lav .lb
#.cv.c:
#	$(VCC) -o $@ $<

.cv.i:
	rm -f $@
	$(VCC) -o $@ $<
	chmod a-w $@

#.cv.o:
#	$(VCC) -o $*.c $<
#	$(CC) $(CFLAGS) -c $*.c
#	rm -f $*.c

# no more joined source files compiling directly to object files,
# see .cv.i
#.cv.o:
#	$(VCC) -o $*.c $<
#	$(CC) $(CFLAGS) -c $*.c; rm -f $*.c

# ПЕРЕВОД СЕКЦИИ ДАННЫХ В СЕКЦИЮ КОДА
# МОЖЕТ ПОНАДОБИТЬСЯ ДЛЯ ОВЕРЛЕЙНЫХ ПРОГРАММ И
# СИСТЕМНЫХ ПРОГРАММ, СОБИРАЕМЫХ С КЛЮЧОМ ld -n,
# см. также п?авило data-to-text:
# .cv.o:
#         $(VCC) -o $*.c $<
#         $(CC) $(CFLAGS) -S $*.c
#         ed - $*.s <data-to-text
#         $(AS) -o $*.o $*.s
#         rm -f $*.c $*.s

.la.lb:
	$(LBP) $< $@

.lav.la:
	$(VLBP) -o $@ $<

.lav.lb:
	$(VLBP) -o $*.la $<
	$(LBP) $*.la $@; rm -f $*.la

all:  $(PROGRAM)

data-to-text:
	echo 'g/\.data/s//\.text/' >data-to-text
	echo 'w' >>data-to-text
	echo 'q' >>data-to-text

program:        $(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS) $(MAKEFILE)
		@echo "Loading $(PROGRAM) ... "
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) $(STDLIBS) -o $(PROGRAM)
		@echo " ...done"

clean:
	rm -f $(PROGRAM) $(OBJS) $(IPAGES) data-to-text $(NOTEXT)
#	cd FORMS; make clean

depend:
	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

calls:  $(SRCS)
	calls $(CFLAGS) $(SRCS) >calls

install:        $(DESTBIN) $(PROGRAM)
		@echo "Install $(PROGRAM) to $(DESTBIN) ..."
		cp $(PROGRAM) $(DESTBIN)
		cd FORMS; make install
		@echo done

update:		$(DESTBIN)/$(PROGRAM)

$(DESTBIN)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		$(MAKE) -f $(MAKEFILE) DEST=$(DEST) install

$(DESTBIN):
	-mkdir -p $(DESTBIN)
	test -d $(DESTBIN)

###
pmain.o: pmain.c

pmainv.o: pmainv.c pmainv.i

#pmainv.c: pmainv.i
