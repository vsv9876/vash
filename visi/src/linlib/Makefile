
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
VERSION       = LINLIB_3

CC            = cc
DEST          = ../../lib
LIBRARY	      = libline.a
MAKEFILE      = Makefile

# CLAGS_VISI наследуется из ../Makefile, возможные значения определены в line.h
CFLAGS_VISI   =
CFLAGS        = $(CFLAGS_VISI) -I../../include
LINT          = lint
CI            = ci -f -n$(VERSION) -m"ВЕРСИЯ $(VERSION)" -sRel
CO            = co
UNLOCK        = rcs -l
VCC           = ../../bin/vcc

EXTHDRS       = ../../include/line.h \
		../../include/line0.h \
		../../include/lineva.h \
		/usr/include/ctype.h \
		../../include/ediag.h \
		/usr/include/sgtty.h \
		/usr/include/signal.h \
		/usr/include/stdio.h \
		/usr/include/sys/ioctl.h

INCLUDE       = /usr/include
HDRS	      =

OBJS          = lin100.o \
		lin110.o \
		lin132.o \
		lin140.o \
		lin145.o \
		lin150.o \
		lin151.o \
		lin180.o \
		lin210.o \
		lin215.o \
		lin220.o \
		lin240.o \
		lin245.o \
		lin301.o \
		lin310.o \
		lin320.o \
		lin330.o \
		lin340.o \
		lin345.o \
		lin360.o

SRCS          = lin100.c \
		lin110.c \
		lin132.c \
		lin140.c \
		lin145.c \
		lin150.c \
		lin151.c \
		lin180.c \
		lin210.c \
		lin215.c \
		lin220.c \
		lin240.c \
		lin245.c \
		lin301.c \
		lin310.c \
		lin320.c \
		lin330.c \
		lin340.c \
		lin345.c \
		lin360.c

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
#lin140.o: /usr/include/stdio.h ../../include/line.h /usr/include/sgtty.h \
#	/usr/include/sys/ioctl.h \
#	/usr/include/signal.h
#lin145.o: /usr/include/stdio.h ../../include/line.h
#lin150.o: /usr/include/stdio.h ../../include/line.h /usr/include/sgtty.h \
#	/usr/include/sys/ioctl.h
#lin210.o: /usr/include/stdio.h ../../include/line.h ../../include/line0.h \
#	../../include/lineva.h
#lin215.o: /usr/include/stdio.h ../../include/line.h ../../include/line0.h \
#	../../include/lineva.h
#lin220.o: ../../include/ediag.h /usr/include/stdio.h ../../include/line.h \
#	../../include/line0.h /usr/include/ctype.h
#lin240.o: ../../include/ediag.h /usr/include/ctype.h /usr/include/stdio.h \
#	../../include/line.h ../../include/line0.h
#lin310.o: /usr/include/stdio.h ../../include/line.h ../../include/line0.h
#lin320.o: /usr/include/stdio.h ../../include/line.h ../../include/line0.h \
#	/usr/include/ctype.h
#lin330.o: /usr/include/stdio.h /usr/include/ctype.h ../../include/line.h \
#	../../include/line0.h
#lin340.o: /usr/include/stdio.h ../../include/line.h ../../include/line0.h
#lin360.o: /usr/include/stdio.h ../../include/line.h ../../include/line0.h
