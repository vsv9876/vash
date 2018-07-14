
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

DEST          = /usr/local

VBIN          = ../../visi/bin
VLBP          = $(VBIN)/vlbp
LBP           = $(VBIN)/lbp

LIBRARY       = ash

MAKEFILE      = Makefile

ASHSTD        = \
		ASHINTRO \
		HELP \
		.ashstd \
		.ashproc \
		.ashhelp \
		.ashmenu \
		.ashmisc \
		.ashcompr

ASHCMD        =


OBJS	      = 1shell.lb\
		2menu.lb\
		3view.lb\
		4edit.lb\
		5copy.lb\
		6movren.lb\
		6movren.lb\
		7misc.lb\
		8remove.lb\
		alias.lb\
		ash.lb\
		mainh.lb\
		other.lb\
		phelp0.lb\
		sup.lb\
		suphlp.lb

SRCS	      = 1shell.lav\
		2menu.lav\
		3view.lav\
		4edit.lav\
		5copy.lav\
		6movren.la\
		6movren.lav\
		7misc.lav\
		8remove.lav\
		alias.lav\
		ash.lav\
		mainh.lav\
		other.lav\
		phelp0.lav\
		sup.lav\
		suphlp.lav

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

install:        $(OBJS) $(DEST) $(DEST)/lib $(DEST)/lib/$(LIBRARY)
#                -rm -f .*.b *.b
		cp $(ASHSTD) $(OBJS) $(DEST)/lib/$(LIBRARY)
#                cp $(ASHCMD) $(DEST)/bin

deinstall:
		-rm -r -f $(DEST)/lib/$(LIBRARY)

slink:          $(OBJS)
		ln -s `pwd` $(DEST)/lib/$(LIBRARY)
$(DEST):
		mkdir $(DEST)
$(DEST)/lib:
		mkdir $(DEST)/lib
$(DEST)/lib/$(LIBRARY):
		mkdir $(DEST)/lib/$(LIBRARY)
###
