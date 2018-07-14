
SHELL         = /bin/sh

##
##      +----------+    βιβμιοτελα χχοδα-χωχοδα
##     (C) LINLIB  !    δμρ αμζαχιτξο-γιζςοχωθ
##      +----------+    χιδεοτεςνιξαμοχ
##
##      $Header$
##      $Log$
##

#
#       σβοςλα πςιλμαδξοκ πςοηςαννω βεϊ υσταξοχλι
#       βιβμιοτελ LINLIB ξα σταξδαςτξωε νεστα

VISI          = ../../../visi
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

PROGRAM       = vf
EXECMODE      =  711
DEST          = /usr/local/bin

CFLAGS        = -DDEMOS2 -I../../../visi/include -DHELP -DSYSV
CC            = cc

LINKER        = cc
LDFLAGS       =
# STDLIBS       = -ltermcap
# STDLIBS       = -ltermnew
LIBS          = \
		$(VLIB)/liblinusr.a \
		$(VLIB)/libline.a \
		$(VLIB)/libtermnew.a

VCC           = $(VBIN)/vcc
VLBP          = $(VBIN)/vlbp
LBP           = $(VBIN)/lbp
MAKEFILE      = Makefile

EXTHDRS	      = ../../../visi/include/line.h \
		/usr/include/ctype.h \
		/usr/include/stdio.h

HDRS	      = vf.h

SRCS          = lin145.c \
		vf.cv

OBJS	      = lin145.o \
		vf.o

SUFFIX        = .cv:sC

.SUFFIXES : .out .o .c .cm .f .e .r .y .yr .ye .l .s .cl .p .q \
	    .cv .la .lav .lb
.cv.c:
	$(VCC) -o $@ $<

.cv.o:
	$(VCC) -o $*.c $<
	$(CC) $(CFLAGS) -c $*.c
	rm -f $*.c

# πεςεχοδ σελγιι δαξξωθ χ σελγιΰ λοδα
# νοφετ ποξαδοβιτψσρ δμρ οχεςμεκξωθ πςοηςανν ι
# σιστενξωθ πςοηςανν, σοβιςαενωθ σ λμΰώον ld -n,
# ΣΝ. ΤΑΛΦΕ ΠÒΑΧΙΜΟ data-to-text:
# .cv.o:
#         $(VCC) -o $*.c $<
#         $(CC) $(CFLAGS) -S $*.c
#         ed - $*.s <data-to-text
#         $(AS) -o $*.o $*.s
#         rm -f $*.c $*.s

.la.lb:
	$(LBP) $< $@

.lav.la:
	$(VLBP) -o $@ $<

.lav.lb:
	$(VLBP) -o $*.la $<
	$(LBP) $*.la $@
	rm -f $*.la

all:  $(PROGRAM)

data-to-text:
	echo 'g/\.data/s//\.text/' >data-to-text
	echo 'w' >>data-to-text
	echo 'q' >>data-to-text

program:        $(PROGRAM)

objs:           $(OBJS)

$(PROGRAM):     $(OBJS) $(LIBS) $(MAKEFILE)
		@echo "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) $(STDLIBS) -o $(PROGRAM)
		@echo " ...done"

clean:
	rm -f $(OBJS) data-to-text $(NOTEXT)

depend:
	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

calls:  $(SRCS)
	calls $(CFLAGS) $(SRCS) >calls

install:        $(DEST) $(PROGRAM)
		cp $(PROGRAM) $(DEST)
		strip $(DEST)/$(PROGRAM)
		chmod $(EXECMODE) $(DEST)/$(PROGRAM)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

$(DEST):
	mkdir $(DEST)
###
lin145.o: /usr/include/stdio.h ../../../visi/include/line.h
vf.o: /usr/include/ctype.h /usr/include/stdio.h \
	../../../visi/include/line.h vf.h
