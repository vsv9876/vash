
SHELL    = /bin/sh

DEST     = /usr/local
DESTDIR  =

VISILIB = $(DESTDIR)$(DEST)/lib/visi/.vhset

#
# 1996 by Sergey Vovk
#
# make install - install system wide visi preferences

info:
	@echo 'Usage:'
	@echo '    make all'
	@echo '    make install'
	@echo '    make clean'

install:    $(DEST) $(VISILIB) .vhset.tar
	cat .vhset.tar | ( cd $(VISILIB); tar -xf - )

$(DEST):
	-mkdir -p $(DEST)
	mkdir $(DEST)

$(VISILIB):
	-mkdir -p $(VISILIB)
