
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

VISI          = ../../visi

VBIN          = $(VISI)/bin

#VLBP          = $(VBIN)/vlbp
#LBP           = $(VBIN)/lbp
VLBP          = vlbp
LBP           = lbp

LIBRARY       = vash

MAKEFILE      = Makefile

PROFILES      = \
		std.help \


ASHCMD        =


OBJS	      = 1shell.lb\
		2menu.lb\
		3view.lb\
		4edit.lb\
		5copy.lb\
		6movren.lb\
		7misc.lb\
		8remove.lb\
		alias.lb\
		ash.lb\
		mainh.lb\
		other.lb\
		phelp0.lb\
		sup.lb\
		suphlp.lb \
		std.help.lb

SRCS	      = 1shell.lav\
		2menu.lav\
		3view.lav\
		4edit.lav\
		5copy.lav\
		6movren.lav\
		7misc.lav\
		8remove.lav\
		alias.lav\
		ash.lav\
		mainh.lav\
		other.lav\
		phelp0.lav\
		sup.lav\
		suphlp.lav \
		std.help.lav

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
