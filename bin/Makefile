
SHELL         = /bin/sh

##
##      +----------+    библиотека ввода-вывода
##     (C) LINLIB  !    для алфавитно-цифровых
##      +----------+    видеотерминалов
##
##      $Header$
##      $Log$
##

#	(v)ash remix, additional scripts

DEST = /usr/local

#include		../conf/Makefile
include		../BLD.cfg

VISI          = ../visi
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

SCRIPTS       = v
#EXECMODE      =  711
EXECMODE      =  755

DESTDIR       =
#DEST          = /usr/local
DESTBIN       = $(DESTDIR)$(DEST)/bin

LIBS          =

MAKEFILE      = Makefile

EXTHDRS	      =

HDRS	      =

SRCS          = 

OBJS	      =

all:  $(SCRIPTS)

scripts:        $(SCRIPTS)

clean:
	rm -f $(SCRIPTS) *~ *.b *.bak

otherclean:

otherinst:
#                cd ./ashrc; make DEST=$(DEST) install

#install:        $(SCRIPTS) $(DESTBIN) otherinst
install:        $(SCRIPTS) $(DESTBIN)
		cp -p $(SCRIPTS) $(DESTBIN)

.sh:	
	cp -p $< $@
	chmod 755 $@

$(DEST)/bin:
	mkdir -p $(DEST)/bin

$(DEST):
	mkdir -p $(DEST)

$(DESTDIR):
	mkdir -p $(DESTDIR)

###

