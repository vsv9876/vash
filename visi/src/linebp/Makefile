
SHELL   = /bin/sh

#
# Прототип файла для сборки и проверки библиотеки;
#
# --Стандартные цели:
# all:          трансляция и сборка в каталоге текстов
# clean:        удаление промежуточных файлов трансляции
# depend:       Настроить Makefile при помощи mkmf;
# dis:          то же, плюс изменить место установки на ../../lib
# std:          -----"------                            /usr/new/lib
# install:      установить библиотеку на место
# update:       полная пересборка и установка
# library:      пересобрать библиотеку
# lintcheck:    проверить тексты библиотеки при помощи lint
# lintlib:      создать псевдо-библиотеку для lint (пока не готово)
# co:           вынуть из архива исходные тексты
# rel:          вынуть из архива исходные тексты версии, указанной ниже
# ci:           положить в архив стабильную версию исх. текстов
# unlock:       снять блокировку для дальнейШей модификации исх. текстов

#последняя стабильная версия: (список см. rlog Makefile)
VERSION       = V3_3

CC            = cc
DEST          = ../../lib
LIBRARY	      = liblinebp.a
MAKEFILE      = Makefile
CFLAGS        = -I../../include
LINT          = lint
CI            = ci -f -n$(VERSION) -m"ВЕРСИЯ $(VERSION)" -sRel
CO            = co
UNLOCK        = rcs -l
VCC           = ../../bin/vcc

EXTHDRS       = ../../include/line.h \
		../../include/linebp.h \
		/usr/include/stdio.h

INCLUDE       = /usr/include
HDRS	      =

OBJS	      = lin505.o \
		lin510.o \
		lin551.o \
		lin552.o \
		lin553.o

SRCS	      = lin505.c \
		lin510.c \
		lin551.c \
		lin552.c \
		lin553.c

SUFFIX        = .cv:sC

.SUFFIXES : .out .o .c .cm .f .e .r .y .yr .ye .l .s .cl .p .q \
	    .cv .la .lav .lb
.cv.c:
	$(VCC) -o $@ $<;

# ПЕРЕВОД СЕКЦИИ ДАННЫХ В СЕКЦИЮ КОДА
# МОЖЕТ ПОНАДОБИТЬСЯ ДЛЯ ОВЕРЛЕЙНЫХ ПРОГРАММ И
# СИСТЕМНЫХ ПРОГРАММ, СОБИРАЕМЫХ С КЛЮЧОМ ld -n
.cv.o:
	$(VCC) -o $*.c $<;
	$(CC) $(CFLAGS) -c $*.c;        #-   - ОБЫЧНАЯ ТРАНСЛЯЦИЯ
	rm -f $*.c;                     #-
#        $(CC) $(CFLAGS) -S $*.c;        #+   + ДЛЯ ОВЕРЛЕЯ
#        ed - $*.s <data-to-text;        #+
#        $(AS) -o $*.o $*.s;             #+
#        rm -f $*.c $*.s;                #+

.la.lb:
	$(LBP) $< $@;

.lav.la:
	$(VLBP) -o $@ $<;

.lav.lb:
	$(VLBP) -o $*.la $<;
	$(LBP) $*.la $@;
	rm -f $*.la;

all:            .co $(LIBRARY)

data-to-text:
	echo 'g/\.data/s//\.text/' >data-to-text;
	echo 'w' >>data-to-text;
	echo 'q' >>data-to-text;

$(LIBRARY):     $(OBJS)
		:     "Loading $(LIBRARY) ... "
		rm -f $(LIBRARY)
		ar cr  $(LIBRARY) $(OBJS)
		-ranlib $(LIBRARY)
		:     "   ...OK"

#$(LIBRARY):     $(OBJS)
#		:     "Loading $(LIBRARY) ... "
#		rm -f $(LIBRARY)
#		ar cr  $(LIBRARY) `lorder $(OBJS) | tsort`
#		-ranlib $(LIBRARY)
#		:     "   ...OK"

clean:;		@rm -f $(OBJS)

depend:;	@mkmf -f $(MAKEFILE) LIBRARY=$(LIBRARY) DEST=$(DEST)

extract:;       @ar x  $(DEST)/$(LIBRARY)
		@rm -f __.SYMDEF

install:        .co $(LIBRARY) $(HDRS)
		@echo Installing $(LIBRARY) in $(DEST)
		mv      $(LIBRARY) $(DEST)
		-ranlib $(DEST)/$(LIBRARY)
		@echo Library $(LIBRARY) installed in $(DEST) , OK.

library:        $(LIBRARY)

lintcheck:;     $(LINT) $(LINTFLAGS) $(CFLAGS) $(SRCS)

update:         $(DEST)/$(LIBRARY)

$(DEST)/$(LIBRARY): $(SRCS) $(HDRS) $(EXTHDRS)
		@-ar xo $(DEST)/$(LIBRARY)
		@make -f $(MAKEFILE) DEST=$(DEST) install

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
		@echo "ИСХОДНЫЕ ТЕКСТЫ ДОСТУПНЫ ДЛЯ МОДИФИКАЦИИ"

# ПОСЛЕ ЭТОЙ СТРОЧКИ НИЧЕГО НЕ ТРОГАТЬ:
#
###
lin505.o: /usr/include/stdio.h ../../include/line.h ../../include/linebp.h
lin510.o: /usr/include/stdio.h ../../include/line.h ../../include/linebp.h
lin551.o: ../../include/line.h ../../include/linebp.h
lin552.o: ../../include/line.h ../../include/linebp.h
lin553.o: ../../include/line.h ../../include/linebp.h
