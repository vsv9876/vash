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

#       οβςαβοτλα πςι πονούι xstr
#       (σν. πςαχιμα .c.o:  .cv.o)

VISI          = ../visi
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

PROGRAM       = ash
EXECMODE      =  711
DEST          = /usr/local

CFLAGS        = -DDEMOS2\
		-I../visi/include\
		-D'DESTLIB="$(DEST)/lib/$(PROGRAM)/"'

CPP           = cc -E -C
CC            = cc -O
LINT          = lint -h -n

LINKER        = cc
LDFLAGS       =
# STDLIBS       = -ltermcap
# STDLIBS       = -ltermnew
STDLIBS       = -ljobs -lndir
LIBS          = \
		libnew/libnew.o \
		$(VLIB)/liblinebp.a \
		$(VLIB)/liblinusr.a \
		$(VLIB)/libline.a \
		$(VLIB)/libtermnew.a

VCC           = $(VBIN)/vcc
VLBP          = $(VBIN)/vlbp
LBP           = $(VBIN)/lbp
MAKEFILE      = Makefile

EXTHDRS	      = ../visi/include/linebp.h \
		/usr/include/a.out.h \
		/usr/include/ar.h \
		/usr/include/ctype.h \
		/usr/include/ndir.h \
		/usr/include/signal.h \
		/usr/include/stdio.h \
		/usr/include/strings.h \
		/usr/include/sys/code.h \
		/usr/include/sys/ioctl.h \
		/usr/include/sys/iopage.h \
		/usr/include/sys/localopts.h \
		/usr/include/sys/param.h \
		/usr/include/sys/psw.h \
		/usr/include/sys/stat.h \
		/usr/include/sys/types.h \
		/usr/include/time.h \
		/usr/include/whoami.h

HDRS	      = assist.h \
		astat.h \
		line.h

SRCS          = Small.c \
		amenu.c \
		filetype.c \
		main.c \
		menu2.c \
		vcmd.c \
		vcmddat.c \
		vcmdh.c \
		vcoprt.c \
		vdcanon.c \
		vex.c \
		vexdir.c \
		vfread.c \
		vmain.c \
		vmisc.c \
		vndir.c \
		vshcmd.c \
		vsusp.c \
		vsystem.c

OBJS	      = Small.o \
		amenu.o \
		filetype.o \
		main.o \
		menu2.o \
		vcmd.o \
		vcmddat.o \
		vcmdh.o \
		vcoprt.o \
		vdcanon.o \
		vex.o \
		vexdir.o \
		vfread.o \
		vmain.o \
		vmisc.o \
		vndir.o \
		vshcmd.o \
		vsusp.o \
		vsystem.o

SUFFIX        = .cv:sC

.SUFFIXES : .out .o .c .cm .f .e .r .y .yr .ye .l .s .cl .p .q \
	    .cv .la .lav .lb
.c.o:
	$(CC) -c $(CFLAGS) $<

.cv.c:
	$(VCC) -o $@ $<

# .cv.o:
#         $(VCC) -o $*.c $<
#         $(CC) $(CFLAGS) -c $*.c
#         rm -f $*.c
#
# πεςεχοδ σελγιι δαξξωθ χ σελγιΰ λοδα
# νοφετ ποξαδοβιτψσρ δμρ οχεςμεκξωθ πςοηςανν ι
# σιστενξωθ πςοηςανν, σοβιςαενωθ σ λμΰώον ld -n,
# ΣΝ. ΤΑΛΦΕ ΠÒΑΧΙΜΟ data-to-text:
.cv.o:
	$(VCC) -o $*.c $<
	$(CC) $(CFLAGS) -S $*.c
	ed - $*.s <data-to-text
	$(AS) -o $*.o $*.s
	rm -f $*.c $*.s

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

objs:           data-to-text $(OBJS)

xs.o:           strings
		xstr
		$(CC) $(CFLAGS) -S $*.c
		ed - $*.s <data-to-text
		$(AS) -o $*.o $*.s
		rm -f $*.s

$(PROGRAM):     data-to-text $(OBJS) xs.o $(LIBS) $(MAKEFILE)
		@echo "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) xs.o $(OBJS) $(LIBS) $(STDLIBS) -o $(PROGRAM)
		@echo " ...done"

clean:  otherclean
	rm -f $(OBJS) xs.c xs.o x.c x.o strings data-to-text $(NOTEXT)

# ζΑΚΜΩ xstr ΞΕ ΔΟΜΦΞΩ ΠΟΠΑΣΤΨ Χ ΣΠΙΣΛΙ
#
depend:
	rm -f xs.? x.?
	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

calls:  $(SRCS)
	calls $(CFLAGS) $(SRCS) >a.calls

lint lintcheck:;        $(LINT) $(CFLAGS) *.c >a.lint

otherclean:
		cd ./LIB; make DEST=$(DEST) clean
		cd ./ashrc; make DEST=$(DEST) clean

other:
		cd ./LIB; make DEST=$(DEST) install
		cd ./ashrc; make DEST=$(DEST) install

install:        $(PROGRAM) $(DEST) $(DEST)/bin other
		cp $(PROGRAM) $(DEST)/bin
		-c7t10 $(DEST)/bin/$(PROGRAM)
		strip $(DEST)/bin/$(PROGRAM)
		chmod $(EXECMODE) $(DEST)/bin/$(PROGRAM)

update:         $(DEST)/bin/$(PROGRAM)

$(DEST)/bin/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

$(DEST)/bin:
	mkdir $(DEST)/bin
$(DEST):
	mkdir $(DEST)
###
amenu.o: /usr/include/stdio.h line.h assist.h
filetype.o: /usr/include/a.out.h /usr/include/ar.h /usr/include/ctype.h \
	/usr/include/stdio.h
main.o: /usr/include/signal.h /usr/include/sys/localopts.h \
	/usr/include/ctype.h /usr/include/stdio.h line.h assist.h
menu2.o: /usr/include/sys/types.h /usr/include/sys/localopts.h \
	/usr/include/strings.h /usr/include/stdio.h line.h assist.h
vcmd.o: /usr/include/ctype.h /usr/include/stdio.h line.h assist.h astat.h
vcmddat.o: /usr/include/strings.h /usr/include/ctype.h /usr/include/stdio.h \
	line.h assist.h astat.h
vcmdh.o: /usr/include/strings.h /usr/include/stdio.h line.h assist.h
vcoprt.o: /usr/include/stdio.h line.h
vdcanon.o: /usr/include/strings.h
vex.o: /usr/include/stdio.h /usr/include/ctype.h line.h \
	../visi/include/linebp.h assist.h astat.h
vfread.o: /usr/include/ndir.h line.h /usr/include/stdio.h assist.h
vmain.o: /usr/include/signal.h /usr/include/sys/localopts.h \
	/usr/include/ctype.h /usr/include/stdio.h line.h assist.h
vmisc.o: /usr/include/sys/types.h /usr/include/sys/localopts.h \
	/usr/include/time.h /usr/include/stdio.h line.h assist.h
vndir.o: /usr/include/sys/types.h /usr/include/sys/localopts.h \
	/usr/include/sys/stat.h /usr/include/strings.h /usr/include/stdio.h \
	/usr/include/ndir.h line.h assist.h astat.h
vshcmd.o: /usr/include/strings.h /usr/include/stdio.h line.h assist.h
vsusp.o: /usr/include/stdio.h line.h /usr/include/signal.h \
	/usr/include/sys/localopts.h
vsystem.o: /usr/include/sys/ioctl.h /usr/include/sys/code.h \
	/usr/include/sys/localopts.h /usr/include/sys/param.h \
	/usr/include/whoami.h /usr/include/signal.h /usr/include/sys/psw.h \
	/usr/include/sys/types.h /usr/include/sys/iopage.h \
	/usr/include/ctype.h /usr/include/stdio.h line.h assist.h
