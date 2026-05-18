
SHELL         = /bin/sh

#
# vash scripts
#

MAKEFILE      = Makefile

#DEST = /usr/local
DEST = /usr

#include		../conf/Makefile
include		../BLD.cfg

VISI          = ../visi
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

SCRIPTS       = v
#EXECMODE      =  711
EXECMODE      =  755

DESTDIR       = ../BLD

DESTBIN       = $(DESTDIR)$(DEST)/bin

all:  $(SCRIPTS)

scripts:        $(SCRIPTS)

clean:
	rm -f $(SCRIPTS) *~ *.b *.bak

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

$(DESTBIN):
	mkdir -p $(DESTBIN)

###
