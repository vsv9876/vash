
SHELL    = /bin/sh

#       +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
#      (C) LINLIB  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
#       +----------+    ВИДЕОТЕРМИНАЛОВ
#
#       Установка компонент в каталоги дистрибутива
#

#       $Header$
#       $Log$

DEST     = ../..
CC       = cc

# наследуется из ../src/Makefile, список возможных значений берется из line.h
#CGLAGS_VISI = -DUSE_TERMIO
#CGLAGS_VISI = -DUSE_SGTTYH
#CGLAGS_VISI = -DUSE_TERMIOS_386BSD
#CGLAGS_VISI = -DUSE_TERMIOS

# нигде не используется CFLAGS в этом Makefile
CFLAGS   =
LINKER   = cc

MAKEFILE = Makefile

ALL      =  lbp  vcc  linebp  linusr  linlib  termnew
ALLINST  = .lbp .vcc .linebp .linusr .linlib .termnew
#ALL      =  lbp  vcc  linebp  linusr  linlib
#ALLINST  = .lbp .vcc .linebp .linusr .linlib

all:            $(ALLINST)

install:        $(ALLINST)

first:
	 -cd ../include; $(MAKE) co
	 sh -cx "for i in $(ALL); do (cd \$$i; $(MAKE) co); done"
	 -cd linlib; mkdir $(DEST)/lib $(DEST)/bin

depend:; sh -cx "for i in $(ALL); do (cd \$$i; $(MAKE) depend); done"

clean:;  sh -cx "for i in $(ALL); do (cd \$$i; $(MAKE) clean); done"
	 rm -f $(ALLINST)

std:    .std

local:  .local

.std:
		mkmf -f $(MAKEFILE) "DEST=/usr"
		: "Makefile настроен:"
		: "Компоненты будут установлены на стандартные места"

.local:
		mkmf -f $(MAKEFILE) "DEST=../.."
		: "Makefile настроен:"
		: "Компоненты будут установлены в рабочем каталоге ./.."

.termnew:;      cd termnew; $(MAKE) install "DEST=$(DEST)/lib" "CC=$(CC)"
		touch .termnew

#.termnew:;      cd termnew; ln -s /usr/lib/libtermcap.a $(DEST)/lib/libtermnew.a
#		touch .termnew

.linlib:;       cd linlib;  $(MAKE) install "DEST=$(DEST)/lib" "CC=$(CC)" \
			"CFLAGS_VISI=$(CFLAGS_VISI)"
		touch .linlib

.linusr:;       cd linusr;  $(MAKE) install "DEST=$(DEST)/lib" "CC=$(CC)"
		touch .linusr

.linebp:;       cd linebp;  $(MAKE) install "DEST=$(DEST)/lib" "CC=$(CC)"
		touch .linebp

.lbp:.linlib;          cd lbp;     $(MAKE) install "DEST=$(DEST)/bin" "CC=$(CC)" "LINKER=$(LINKER)"
		touch .lbp

.vcc:.linlib;          cd vcc;    $(MAKE) install  "DEST=$(DEST)/bin" "CC=$(CC)" "LINKER=$(LINKER)"
		touch .vcc

$(DEST)/include:
		mkdir $(DEST)/include

$(DEST)/lib:
		mkdir $(DEST)/lib

$(DEST)/bin:
		mkdir $(DEST)/bin

###
