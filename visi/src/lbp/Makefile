
SHELL   = /bin/sh

#
# Прототип файла для сборки и проверки программы;
#
# --Стандартные цели:
# all:          трансляция и сборка в каталоге текстов
# program:      --- "" ---
# clean:        удаление промежуточных файлов трансляции
# depend:       Настроить Makefile при помощи mkmf;
# dis:          то же, плюс изменить место установки на ../../bin
# std:          --- "" ---                              /usr/new/bin
# install:      установить программу на место
# lintcheck:    проверить тексты программы при помощи lint (пока не готово)
# update:       полная пересборка и установка
# co:           вынуть из архива исходные тексты
# rel:          вынуть из архива исходные тексты версии, указанной ниже
# ci:           положить в архив стабильную версию исх. текстов
# unlock:       снять блокировку для дальнейШей модификации исх. текстов

VERSION       = LINLIB_3

PROGRAM	      = lbp

#
#       СБОРКА ПРИКЛАДНОЙ ПРОГРАММЫ БЕЗ УСТАНОВКИ
#       БИБЛИОТЕК LINLIB НА СТАНДАРТНЫЕ МЕСТА
#VISI          = ../../../visi
VISI          = ../..
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

CC            = cc
CFLAGS        = -I../../include
LINKER        = cc
LDFLAGS       =

LIBS          = \
		$(VLIB)/libline.a

DEST	      = ../../bin
MAKEFILE      = Makefile
CI            = ci -f -n$(VERSION) -m"ВЕРСИЯ $(VERSION)" -sRel
CO            = co
UNLOCK        = rcs -l
VCC           = ../../bin/vcc


EXTHDRS	      = ../../include/line.h \
		../../include/linebp.h \
		/usr/include/ctype.h \
		/usr/include/stdio.h

HDRS	      =

OBJS	      = lbp.o

SRCS	      = lbp.c

all:            .co $(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		:     "Loading $(PROGRAM) ... "
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		:     " ...done"

clean:;         rm -f $(OBJS)

depend:;        mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

install:        .co $(PROGRAM)
		: ---  Installing $(PROGRAM) in $(DEST)
		mv $(PROGRAM) $(DEST)

program:        .co $(PROGRAM)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		make -f $(MAKEFILE) DEST=$(DEST) install

$(DEST):;       mkdir $(DEST)


rel:;   $(CO) -r$(VERSION) $(SRCS)
	date >.co

co:     .co

.co:;   $(CO) $(SRCS)
	date >.co

ci:;    $(CI) $(SRCS)
	rm -f .co

unlock:         .co .unlock

.unlock:;       $(UNLOCK) $(SRCS)
		:     "ИСХОДНЫЕ ТЕКСТЫ ДОСТУПНЫ ДЛЯ МОДИФИКАЦИИ"
#
# ПОСЛЕ ЭТОЙ СТРОЧКИ НИЧЕГО НЕ ТРОГАТЬ:
#
###
lbp.o: /usr/include/stdio.h /usr/include/ctype.h ../../include/line.h \
	../../include/linebp.h
