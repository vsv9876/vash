
SHELL         = /bin/sh

##      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
##     (C) LINLIB  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
##      +----------+    ВИДЕОТЕРМИНАЛОВ

#       СБОРКА ПРИКЛАДНОЙ ПРОГРАММЫ БЕЗ УСТАНОВКИ
#       БИБЛИОТЕК LINLIB НА СТАНДАРТНЫЕ МЕСТА

# Стандартные цели для этого маке-файла:
#
# all      - все перетранслировать и пересобрать
# program  -    то же самое (собрать программу)
# clean    -    удалить промежуточные файлы трансляции
# depend   -    перестроить списки файлов (через mkmf)
# install  -    установить программу на стандартное место
# update   -    обновить программу (если поменялись системные компоненты)

VBIN          = ../../bin
VLIB          = ../../lib

PROGRAM       = proto
EXECMODE      =  711
DEST          = /usr/new/bin

CFLAGS        = -DDEMOS2 -I../../include
CC            = cc

LINKER        = cc

include ../../../conf/Makefile
LDFLAGS       =

# Рекомендуется пользоваться исправленной библиотекой termcap:
# STDLIBS       = -ltermcap
LIBS          = $(VLIB)/liblinusr.a $(VLIB)/libline.a $(VLIB)/libtermnew.a
VCC           = $(VBIN)/vcc
VLBP          = $(VBIN)/vlbp
LBP           = $(VBIN)/lbp
MAKEFILE      = Makefile

EXTHDRS       = ../../include/line.h \
		/usr/include/stdio.h

HDRS	      =

SRCS          = pmain.c \
		pmainv.cv

OBJS	      = pmain.o \
		pmainv.o

SUFFIX        = .cv:sC

.SUFFIXES : .out .o .c .cm .f .e .r .y .yr .ye .l .s .cl .p .q \
	    .cv .la .lav .lb
.cv.c:
	$(VCC) -o $@ $<;

.cv.o:
	$(VCC) -o $*.c $<;
	$(CC) $(CFLAGS) -c $*.c;
	rm -f $*.c;

all:    $(PROGRAM)

program:        $(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS) $(MAKEFILE)
		@echo "Сборка $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) $(LIBS) $(STDLIBS) -o $(PROGRAM)
		@echo " ...готово"

clean:;         @rm -f $(OBJS)

depend:;        @mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

install:	$(PROGRAM)
		@echo "== УСТАНОВКА $(PROGRAM) В КАТАЛОГ $(DEST)
		@install -s -m $(EXECMODE) $(PROGRAM) $(DEST)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

###
pmain.o: /usr/include/stdio.h ../../include/line.h
pmainv.o: /usr/include/stdio.h ../../include/line.h
