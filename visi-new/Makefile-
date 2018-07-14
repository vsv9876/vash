
SHELL         = /bin/sh

#
# Сборка вспомогательной библиотеки для assist.
#
# Библиотека содержит новейшие версии некоторых модулей LINLIB,
# которые пока не внесены в стандартные библиотеки LINLIB.
# Во избежание проблем с неразрешенными ссылками используется
# не формат ar, a format a.out, для чего вызывается ld -r.
# См. ниже правило $(LIBRARY):
#

MAKEFILE      = Makefile

CFLAGS        = -I.. -I../../visi/include -DW_PAGE_TAB

CC            = cc -O

LIBRARY       = libnew.a

HDRS          =

EXTHDRS       =

SRCS          = lin330.c\
		lin340.c\
		lin360.c\
		lin425.c\
		lin460.c\
		lin472.c

OBJS          = lin330.o\
		lin340.o\
		lin360.o\
		lin425.o\
		lin460.o\
		lin472.o

# ld -r -x -o $(LIBRARY) $(OBJS)
#$(LIBRARY):     $(OBJS)
#		@echo "Linking $(LIBRARY) ... "
#		ld -r -o $(LIBRARY) $(OBJS)
#		@echo "   ...OK"

$(LIBRARY):     $(OBJS)

#		:     "Loading $(LIBRARY) ... "
#		rm -f $(LIBRARY)
#		ar cr  $(LIBRARY) `lorder $(OBJS) | tsort`
#		-ranlib $(LIBRARY)
#		:     "   ...OK"

library:        $(LIBRARY)

all:		library

clean:;         rm -f $(OBJS) $(LIBRARY)

depend:;        mkmf -f $(MAKEFILE)
###
