
SHELL         = /bin/sh

#
# Assistant for sh
# Startup script installation

PROGRAM       = ashrc
EXECMODE      =  755
DEST          = /usr/local

MAKEFILE      = Makefile

SRC           = .ashrc

all:            $(PROGRAM)

clean:
	rm -f *.b .,re* $(PROGRAM)

depend:
	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

.ashrc:
		@echo "File .ashrc absent! Sorry..."; exit 1

install:        $(DEST) $(DEST)/bin .ashrc
		cp .ashrc $(DEST)/bin/$(PROGRAM)
		chmod $(EXECMODE) $(DEST)/bin/$(PROGRAM)

update:         $(DEST)/bin/$(PROGRAM)

$(DEST)/bin/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

.DEFAULT:
	mkdir $@
###
