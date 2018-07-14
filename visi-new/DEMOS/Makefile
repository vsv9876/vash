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

CFLAGS        = -O -DDEMOS2 -I.. -I../../visi/include -DW_PAGE_TAB

LIBRARY       = libnew.o

HDRS          =

EXTHDRS       = ../../visi/include/line0.h \
		../line.h \
		/usr/include/ctype.h \
		/usr/include/stdio.h

SRCS          = lin330.c \
		lin340.c \
		lin360.c \
		lin425.c \
		lin460.c

OBJS          = lin330.o \
		lin340.o \
		lin360.o \
		lin425.o \
		lin460.o

$(LIBRARY):     $(OBJS)
		@echo "Linking $(LIBRARY) ... "
		ld -r -x -o $(LIBRARY) $(OBJS)
		@echo "   ...OK"

library:        $(LIBRARY)

clean:;         rm -f $(OBJS)

depend:;        mkmf -f $(MAKEFILE)
###
lin330.o: /usr/include/stdio.h /usr/include/ctype.h ../line.h \
	../../visi/include/line0.h
lin340.o: /usr/include/stdio.h ../line.h ../../visi/include/line0.h
lin360.o: /usr/include/stdio.h ../line.h ../../visi/include/line0.h
lin425.o: /usr/include/stdio.h /usr/include/ctype.h ../line.h \
	../../visi/include/line0.h
lin460.o: /usr/include/stdio.h ../line.h
