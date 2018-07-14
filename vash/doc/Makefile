
SHELL         = /bin/sh

LIBRARY       = ash

BOOK          = $(LIBRARY).lp

SRCS          = ash_0.me \
		ash_1.me \
		ash_2.me

SUFFIX = .me:s

.SUFFIXES: .me .lp

ash_0.lp: $(SRCS)
	nroff -me $(SRCS) > $(BOOK)

archive:
	tar cvf $(LIBRARY)doc.tar Makefile $(SRCS)

clean:
	rm -f *.lp *.b


###
