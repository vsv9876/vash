
SHELL         = /bin/sh

#
#
#       Руководство по LINLIB
#
#
#

SRC     = \
	head.me \
	intro.me \
	port.me \
	bex.me \
	fin.me

all:    reclam.lp book.lp

install: all

book.lp: $(SRC)
	nroff -me $(SRC) >book.lp

reclam.lp:  reclam.me
	    nroff -me reclam.me >reclam.lp
