
SHELL         = /bin/sh

##
##      +----------+    βιβμιοτελα χχοδα-χωχοδα
##     (C) LINLIB  !    δμρ αμζαχιτξο-γιζςοχωθ
##      +----------+    χιδεοτεςνιξαμοχ
##

##
##      $Header$
##
##      $Log$
##

#       ξαδο ιϊνεξιτψ σοδεςφινοε λαταμοηοχ cat?, ζακμα /usr/lib/whatis

MAKEFILE      = Makefile

CWD           = /mnt/src/users/vsv/visi/doc/man

MAN           = /usr/man

CATMAN        = /usr/man/man0/catman

all:    depend catman

depend: .depend

.depend:; mkmf -f $(MAKEFILE) "CWD=`pwd`"
	  mkdir cat1 cat3 cat5
	  touch .depend

catman:; export WHATIS MAN CAT; \
	 WHATIS=$(CWD)/Whatis; \
	 MAN=$(CWD); \
	 CAT=$(CWD); \
	 $(CATMAN) -n 1; \
	 $(CATMAN) -n 3; \
	 $(CATMAN) -n 5;

clean:;  rm -f cat1/* cat3/* cat5/* .depend

install: depend catman man.i whatis.i

man.i:;
	cd cat1; cp *.* $(MAN)/cat3
	cd cat3; cp *.* $(MAN)/cat3
	cd cat5; cp *.* $(MAN)/cat5

whatis.i:;
	cat /usr/lib/whatis Whatis | sort - | uniq >whatis;
	mv whatis /usr/lib;
###
