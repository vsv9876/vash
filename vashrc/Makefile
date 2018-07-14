
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

include		../conf/Makefile

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
	rm -f *~ *.b *.bak

otherclean:

otherinst:
#                cd ./ashrc; make DEST=$(DEST) install

#install:        $(SCRIPTS) $(DESTBIN) otherinst
install:        $(SCRIPTS) $(DESTBIN)
		chmod $(EXECMODE) $(SCRIPTS)
		cp -p $(SCRIPTS) $(DESTBIN)
# 		chmod $(EXECMODE) $(DESTBIN)/$(PROGRAM)
# 		mv $(DESTBIN)/$(PROGRAM).new $(DESTBIN)/$(PROGRAM)

$(DEST)/bin:
	mkdir -p $(DEST)/bin

$(DEST):
	mkdir -p $(DEST)

$(DESTDIR):
	mkdir -p $(DESTDIR)

###
