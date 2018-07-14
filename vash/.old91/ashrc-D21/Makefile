#
# Assistant for sh
#

PROGRAM       = ashrc
EXECMODE      =  711
DEST          = /usr/local

CFLAGS        = -O -D'DESTBIN="$(DEST)/bin"' -D'DESTLIB="$(DEST)/lib"'
CC            = cc

LINKER        = cc
LDFLAGS       = -n
STDLIBS       =
LIBS          =

MAKEFILE      = Makefile

EXTHDRS	      = /usr/include/strings.h

HDRS	      =

SRCS          = ashrc.c

OBJS	      = ashrc.o

all:    $(PROGRAM)

data-to-text:
	echo 'g/\.data/s//\.text/' >data-to-text
	echo 'w' >>data-to-text
	echo 'q' >>data-to-text

program:        $(PROGRAM)

x.c:            $(SRCS)
		xstr -c $(SRCS)

xs.c:           x.c
		xstr

x.o:    x.c
	$(CC) $(CFLAGS) -c $<

xs.o:   xs.c
	$(CC) $(CFLAGS) -S $<
	ed - $*.s <data-to-text
	$(AS) -o $*.o $*.s
	rm -f $*.s

$(PROGRAM):     data-to-text xs.o x.o $(LIBS) $(MAKEFILE)
		@echo "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) xs.o x.o $(LIBS) $(STDLIBS) -o $(PROGRAM)
		@echo " ...done"

clean:
	rm -f data-to-text x.? xs.? strings $(PROGRAM)

depend:
	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

calls:  $(SRCS)
	calls $(CFLAGS) $(SRCS) >calls

.ashrc:
		@echo "File .ashrc absent"; exit 1

install:        $(DEST) $(DEST)/lib $(DEST)/bin .ashrc $(PROGRAM)
		cp $(PROGRAM) $(DEST)/bin
		-c7t10 $(DEST)/bin/$(PROGRAM)
		-strip $(DEST)/bin/$(PROGRAM)
		-chmod $(EXECMODE) $(DEST)/bin/$(PROGRAM)
		cp .ashrc $(DEST)/lib

update:         $(DEST)/bin/$(PROGRAM)

$(DEST)/bin/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

.DEFAULT:
	mkdir $@
###
ashrc.o: /usr/include/strings.h
