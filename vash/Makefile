
SHELL         = /bin/sh

##
##      +----------+    ΒΙΒΜΙΟΤΕΛΑ ΧΧΟΔΑ-ΧΩΧΟΔΑ
##     (C) LINLIB  !    ΔΜΡ ΑΜΖΑΧΙΤΞΟ-ΓΙΖÒΟΧΩΘ
##      +----------+    ΧΙΔΕΟΤΕÒΝΙΞΑΜΟΧ
##
##      $Header$
##      $Log$
##

#       σβοςλα πςιλμαδξοκ πςοηςαννω βεϊ υσταξοχλι
#       βιβμιοτελ LINLIB ξα σταξδαςτξωε νεστα

#       οβςαβοτλα πςι πονούι xstr
#       (σν. πςαχιμα .c.o:  .cv.o)

#	(v)ash remix

include		../conf/Makefile

VISI          = ../visi
VBIN          = $(VISI)/bin
VLIB          = $(VISI)/lib

VISI_NEW      = ../visi-new

PROGRAM       = vash
#EXECMODE      =  711
EXECMODE      =  755

DESTDIR       =
#DEST          = /usr/local
DESTLIB       = $(DESTDIR)$(DEST)/lib/$(PROGRAM)
DESTBIN       = $(DESTDIR)$(DEST)/bin

#CFLAGS_ASH    = -DJCLsuspend -DSYSV -DSVR4 -DMGROUPS
#CFLAGS_ASH    = -DJCLsuspend -DFbsd -DBSDpagesize -DBSDmagic
#CFLAGS_ASH    =
CFLAGS        = $(CFLAGS_ASH) \
		-I$(VISI)/include\
		-D'DESTLIB="$(DESTLIB)"'

#CC            = cc -O
#LINKER        = cc
CPP           = $(CC) -E -C
LINT          = lint -h -n

LDFLAGS       =

# STDLIBS       = -ltermcap
# STDLIBS       = -ltermnew
# STDLIBS       = -ljobs -lndir
STDLIBS       = -ltermcap

#		$(VISI_NEW)/libnew.a \
#
LIBS          = \
		$(VLIB)/liblinebp.a \
		$(VLIB)/liblinusr.a \
		$(VLIB)/libline.a \


#		$(VLIB)/libtermnew.a

#VCC           = $(VBIN)/vcc
#VLBP          = $(VBIN)/vlbp
#LBP           = $(VBIN)/lbp
VCC           = vcc
VLBP          = vlbp
LBP           = lbp
MAKEFILE      = Makefile

EXTHDRS	      =

HDRS	      = assist.h\
		astat.h

SRCS          = buildd.c\
		amenu.c\
		filetype.c\
		gidchk.c \
		main.c\
		menu2.c\
		vcmd.c\
		vcmddat.c\
		vcmdh.c\
		vcoprt.c\
		vdcanon.c\
		vex.c\
		vexdir.c\
		vfread.c\
		vmain.c\
		vmisc.c\
		vndir.c\
		vshcmd.c\
		vsusp.c\
		vsystem.c

OBJS	      = \
		buildd.o\
		amenu.o\
		filetype.o\
		gidchk.o\
		main.o\
		menu2.o\
		vcmd.o\
		vcmddat.o\
		vcmdh.o\
		vcoprt.o\
		vdcanon.o\
		vex.o\
		vexdir.o\
		vfread.o\
		vmain.o\
		vmisc.o\
		vndir.o\
		vshcmd.o\
		vsusp.o\
		vsystem.o

LIBNEW       = \
		$(VISI_NEW)/lin330.o\
                $(VISI_NEW)/lin340.o\
                $(VISI_NEW)/lin360.o\
                $(VISI_NEW)/lin425.o\
                $(VISI_NEW)/lin460.o\
                $(VISI_NEW)/lin472.o
 

SUFFIX        = .cv:sC

.SUFFIXES : .out .o .c .cm .f .e .r .y .yr .ye .l .s .cl .p .q \
	    .cv .la .lav .lb
.c.o:
	$(CC) -c $(CFLAGS) $<

.c.s:
	$(CC) -S $(CFLAGS) $<

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
# ΣΝ. ΤΑΛΦΕ Π?ΑΧΙΜΟ data-to-text:
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
	$(LBP) $*.la $@; rm -f $*.la

all:  $(PROGRAM)

# data-to-text:
#         echo 'g/\.data/s//\.text/' >data-to-text
#         echo 'w' >>data-to-text
#         echo 'q' >>data-to-text

program:        buildd $(PROGRAM)

objs:           $(OBJS)

# xs.o:           strings
#                 xstr
#                 $(CC) $(CFLAGS) -S $*.c
#                 ed - $*.s <data-to-text
#                 $(AS) -o $*.o $*.s
#                 rm -f $*.s

$(PROGRAM):     $(VISI_NEW)/libnew $(OBJS) $(LIBS) $(MAKEFILE)
		@echo "Loading $(PROGRAM) ... "
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBNEW) $(LIBS) $(STDLIBS) -o $(PROGRAM)
		@echo " ...done"

clean:  otherclean
	rm -f $(OBJS) $(PROGRAM)

# ζΑΚΜΩ xstr ΞΕ ΔΟΜΦΞΩ ΠΟΠΑΣΤΨ Χ ΣΠΙΣΛΙ
#
depend:
	rm -f xs.? x.?
	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

calls:  $(SRCS)
	calls $(CFLAGS) $(SRCS) >a.calls

cxref:  $(SRCS)
	cxref $(CFLAGS) $(SRCS) >a.cxref

lint lintcheck:;        $(LINT) $(CFLAGS) *.c $(VISI_NEW)/*.c >a.lint

otherclean:
		cd ./$(VISI_NEW); make clean;
#                cd ./ashrc; make DEST=$(DEST) clean

otherinst:
#                cd ./ashrc; make DEST=$(DEST) install

#                 -c7t10 $(DEST)/bin/$(PROGRAM)
# install:        $(PROGRAM) $(DEST) $(DEST)/bin
#install:        $(PROGRAM) $(DEST) $(DEST)/bin otherinst
#		-mv $(DEST)/bin/$(PROGRAM) $(DEST)/bin/$(PROGRAM).old
#		cp $(PROGRAM) $(DEST)/bin
#		-strip $(DEST)/bin/$(PROGRAM)
#		-c7t10 $(DEST)/bin/$(PROGRAM)
#		chmod $(EXECMODE) $(DEST)/bin/$(PROGRAM)
#		-rm -f $(DEST)/bin/$(PROGRAM).old

install:        $(PROGRAM) $(DESTBIN) otherinst
		cp $(PROGRAM) $(DESTBIN)/$(PROGRAM).new
		chmod $(EXECMODE) $(DESTBIN)/$(PROGRAM).new
		mv $(DESTBIN)/$(PROGRAM).new $(DESTBIN)/$(PROGRAM)

update:         $(DEST)/bin/$(PROGRAM)

$(DEST)/bin/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		$(MAKE) -f $(MAKEFILE) DEST=$(DEST) "CC=$(CC)" install

$(DEST)/bin:
	mkdir -p $(DEST)/bin

$(DEST):
	mkdir -p $(DEST)

$(DESTDIR):
	mkdir -p $(DESTDIR)
	
	
# rebuild buildd.c, make new timestamp of build date
buildd:
	$(SHELL) ./builddate

buildd.h: buildd
#
#
# portable rulez
#
#$(VISI_NEW)/libnew.a:
#       cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" libnew.a

$(VISI_NEW)/libnew:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" library

$(VISI_NEW)/lib330.o:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" lib330.o

$(VISI_NEW)/lib340.o:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" lib340.o

$(VISI_NEW)/lib360.o:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" lib360.o

$(VISI_NEW)/lib425.o:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" lib425.o

$(VISI_NEW)/lib460.o:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" lib460.o

$(VISI_NEW)/lib472.o:
	cd $(VISI_NEW); make "DEST=$(DEST)" "CC=$(CC)" lib472.o

###
amenu.o: assist.h
buildd.o: buildd.h
main.o: assist.h
menu2.o: assist.h
vcmd.o: assist.h astat.h
vcmddat.o: assist.h astat.h
vcmdh.o: assist.h
vex.o: assist.h astat.h
vfread.o: assist.h
vmain.o: assist.h
vmisc.o: assist.h
vndir.o: assist.h astat.h
vshcmd.o: assist.h
vsystem.o: assist.h
