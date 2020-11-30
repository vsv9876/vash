
SHELL         = /bin/sh

#
# vash profiles to be installed
#

DESTDIR       =

DEST          = /usr
DESTLIB       = $(DESTDIR)$(DEST)/lib

VISI          = ../visi

VBIN          = $(VISI)/bin

LIBRARY       = vash

MAKEFILE      = Makefile

# base names without any suffixes, used in 'install' target only
PROFILES        = \
		std \
		rpm \
		proc \
		menu \
		misc \
		compr

ASHCMD        =

all:            $(OBJS)

form:           $(OBJS)

clean:;         rm -f $(OBJS)

depend:;        mkmf -f $(MAKEFILE) DEST=$(DEST)

#install:        $(PROFILES) $(DESTLIB)/$(LIBRARY)
install:        $(DESTLIB)/$(LIBRARY)
		#chmod +x ./Make.inst.sh
		sh Make.inst.sh $(DESTLIB)/$(LIBRARY) $(PROFILES)

$(DEST):
		mkdir -p $(DEST)
$(DESTLIB):
		mkdir -p $(DESTLIB)
$(DESTLIB)/$(LIBRARY):
		mkdir -p $(DESTLIB)/$(LIBRARY)
###
