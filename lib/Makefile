
SHELL         = /bin/sh

#
# LINLIB V3.3
# ТРАНСЛЯЦИЯ И УСТАНОВКА ФАЙЛОВ ВНЕШНИХ ОПИСАНИЙ
#

# стандартные цели:
# all      - перетранслировать описания страниц;
# form     - то же, что и "all";
# install  - установить файлы страниц в каталог LINLIB-программ;
# slink    - сделать символическую ссылку на рабочий каталог
#            (взамен "install"),
#            может потребоваться при частых исправлениях;
# clean    - очистить каталог, оставить только исходные тексты;
# depend   - перенастроить Makefile при помощи mkmf (обновить списки файлов);

DESTDIR       =

DEST          = /usr
DESTLIB       = $(DESTDIR)$(DEST)/lib

#VISI          = ../../visi
VISI          = ../visi

VBIN          = $(VISI)/bin

#VLBP          = $(VBIN)/vlbp
#LBP           = $(VBIN)/lbp
VLBP          = vlbp
LBP           = lbp

LIBRARY       = vash

MAKEFILE      = Makefile

PROFILES      = \
		std_help \


ASHCMD        =


OBJS	      = about_vash.lb\
		phelp0.lb\
		sup.lb\
		sup_help.lb\
		std_help.lb\
		excode_help.lb

SRCS	      = about_vash.lav\
		phelp0.lav\
		sup.lav\
		sup_help.lav\
		std_help.lav\
		excode_help.lav

SUFFIX        = .lav:s .la:s .lb:o

.SUFFIXES : .lav .la .lb

.la.lb:
	$(LBP) $< $@

.lav.la:
	$(VLBP) -o $@ $<

.lav.lb:
	$(VLBP) -o $*.la $<
	$(LBP) $*.la $@; rm -f $*.la

all:            $(OBJS)

form:           $(OBJS)

clean:;         rm -f $(OBJS)

depend:;        mkmf -f $(MAKEFILE) DEST=$(DEST)

install:        $(OBJS) $(DESTLIB)/$(LIBRARY)
#                -rm -f .*.b *.b
		cp $(OBJS) $(DESTLIB)/$(LIBRARY)
#                cp $(ASHCMD) $(DESTBIN)
		chmod +x ./Make.inst.sh
		sh Make.inst.sh $(DESTLIB)/$(LIBRARY) $(PROFILES)

deinstall:
		-rm -r -f $(DEST)/lib/$(LIBRARY)

#slink:          $(OBJS)
#		ln -s `pwd` $(DEST)/lib/$(LIBRARY)

$(DEST):
		mkdir -p $(DEST)
$(DESTLIB):
		mkdir -p $(DESTLIB)
$(DESTLIB)/$(LIBRARY):
		mkdir -p $(DESTLIB)/$(LIBRARY)
###
