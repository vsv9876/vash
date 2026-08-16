
SHELL = /bin/sh

#
# Visual Assistant Shell
# 1990-2026 (C) Sergey Vovk <vsv>
#
# GPL and/or MIT License
# 

#
# main parameters defined by running script ./configure
#
TOPDIR = .
BLD	= $(TOPDIR)/BLD
DESTDIR	= $(TOPDIR)/BLD
VERSN	= "1."
# distribution dependent (eg /usr/local for FreeBSD)
DEST  = /usr

# main config file (name hard-coded inside ./configure)
BLDCFG	= BLD.cfg

include $(BLDCFG)

VISI  = visi
VISILIB = $(DESTDIR)$(DEST)/lib/visi

VHSET	= ./vhset
VASH	= ./vash
VASHLIB	= ./lib
VEXDIR  = $(DEST)/lib/vash

VASH_DOC = $(DESTDIR)$(DEST)/share/doc/$(VASH)

DOC_FILES = \
	COPYING \
	COMPILE \
	README \
	WISHES \
	NEWS \
	PLAN \


MAN_FILES = \
	vash.1 \
	vhset.1 \


MAN_DIR = $(DESTDIR)$(DEST)/share/man/man1

all:	setup compile

help:
	@echo	'Please, check and edit file '$(BLDCFG)', then'
	@echo	''
	@echo	'    make config        # create '$(BLDCFG)''
	@echo	'    make install       # compile and prepare binaries for packaging'
	@echo	'or'
	@echo	'    make all           # compile only'
	@echo	'    make clean         # cleanup sources tree'
	@echo	'    make distclean     # cleanup distribution sources'
	@echo	'    make showconfig    # print configuration'

#@set -x;
setup:
	@if [ -s "$(BLDCFG)" ]; then \
		true; \
	else \
		echo '***************************' ; \
		echo '' ; \
		echo '  ' Please,; \
		echo '         '$(MAKE) config ; \
		echo '' ; \
		echo '***************************' ; \
	false; \
	fi   

$(BLDCFG):
	touch $(BLDCFG)

prep config:;
	./configure

showconfig:
	@echo '***** '$(BLDCFG)' variables configured:'
	@echo PATH=$(PATH)
	@echo DEST=$(DEST)
	@echo DESTDIR=$(DESTDIR)
	@echo TOPDIR=$(TOPDIR)
	@echo PKGBLD=$(PKGBLD)
	@echo CC=$(CC)
	@echo LINKER=$(LINKER)
	@echo VISI=$(VISI)
	@echo TERMLIB=$(TERMLIB)
	@echo CFLAGS_ASH=$(CFLAGS_ASH)
	@echo CFLAGS_VISI=$(CFLAGS_VISI)
	@echo VERSION=$(VERSION)
	@echo VERSN=$(VERSN)

# $(BLDCFG) $(VISI)/include/line.h $(VISILIB_LIST)
visi_lib: setup
	mkdir -p $(VISI)/lib
	mkdir -p $(VISI)/bin
	cd $(VISI)/src;        $(MAKE) install "CFLAGS_VISI=$(CFLAGS_VISI)"\
		"CC=$(CC)" "LINKER=$(LINKER)"

# NOTE:
# at least FreeBSD's make requires PATH= specified explicitly
compile: setup visi_lib $(BLDCFG) $(VISI)/include/line.h lib
	cd $(VHSET); $(MAKE) all "PATH=$(PATH)" "DEST=$(DEST)"
	cd $(VASH);   $(MAKE) all "PATH=$(PATH)" "DEST=$(DEST)" "ASHLIB=$(ASHLIB)"\
				"CFLAGS_ASH=$(CFLAGS_ASH)" "VERSN=$(VERSN)" "VEXDIR=$(VEXDIR)"
	cd lib;               $(MAKE) "PATH=$(PATH)" "DEST=$(DEST)" all
#	cd lib/LIB;           $(MAKE) "DEST=$(DEST)" all

#install:   $(DESTDIR) $(DEST) termcap $(VISILIB)
install: setup compile $(DESTDIR) $(DEST) $(VISILIB) docinstall maninstall
	cd $(VHSET);	$(MAKE) install "PATH=$(PATH)" \
		CFLAGS_VISI="$(CFLAGS_VISI)" "DEST=$(DEST)" "DESTDIR=$(DESTDIR)"
	cd $(VASH);	$(MAKE) install "PATH=$(PATH)" "VEXDIR=$(VEXDIR)" \
		"CFLAGS_ASH=$(CFLAGS_ASH)" "DEST=$(DEST)" "DESTDIR=$(DESTDIR)"
	cd rc;	$(MAKE) install "PATH=$(PATH)" \
		"DEST=$(DEST)" "DESTDIR=$(DESTDIR)" "PATH=$(PATH)"
	cd bin;	$(MAKE) install "PATH=$(PATH)" \
		"DEST=$(DEST)" "DESTDIR=$(DESTDIR)" "PATH=$(PATH)"
	cd lib;	$(MAKE) install "PATH=$(PATH)" \
		"DEST=$(DEST)" "DESTDIR=$(DESTDIR)" "PATH=$(PATH)"
#	cd lib/LIB;	$(MAKE) install "DEST=$(DEST)"  "DESTDIR=$(DESTDIR)"
#	cp -rp termcap/. $(VISILIB)
#	cd visilib;			$(MAKE) install "DEST=$(DEST)" "DESTDIR=$(DESTDIR)"

# rc contains nothing to be clean
clean:
	cd $(VISI)/bin; rm -f *
	cd $(VISI)/lib; rm -f *
	cd $(VISI)/src; $(MAKE) clean
	cd $(VHSET); $(MAKE) clean
	cd $(VASH); $(MAKE) clean
	cd lib; $(MAKE) clean
	cd vtest; $(MAKE) clean
	cd misc; $(MAKE) clean
#	cd lib/LIB; $(MAKE) clean
	rm -rf $(BLD)

distclean:	clean
	find . -type f -a '(' \
			-name core -o \
			-name '*.b' -o \
			-name '*~' -o \
			-name 'core.*' -o \
			-name '*.core' -o \
			-name a \
		')' -exec rm -f '{}' ';'
	rm -rf $(BLD)
	rm -f  $(BLDCFG) 
	touch $(BLDCFG)

maninstall: $(MAN_FILES) $(MAN_DIR)
	cp $(MAN_FILES) $(MAN_DIR)
	gzip -f $(MAN_DIR)/*

docinstall: $(VASH_DOC)
	cp $(DOC_FILES) $(VASH_DOC)

$(DEST):
	mkdir -p $(DEST)

$(DESTDIR):
	mkdir -p $(DESTDIR)

$(VISILIB):
	mkdir -p $(VISILIB)

$(VASH_DOC):
	mkdir -p $(VASH_DOC)

$(MAN_DIR):
	mkdir -p $(MAN_DIR)

