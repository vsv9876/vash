
SHELL = /bin/sh

# this is permanent, other value not tested yet.
# TODO: config files directory must be configured in runtime and separated from binary installation directory
#DEST  = /usr/local
#DEST  = /usr

VISI  = visi

PKGBLD =

#
# Visual Assistant SHell, ASCII remix
# 2017 Sergey Vovk
# 
#
# NOTE:
# this Makefile intended to setup common compiler flags only,
# via standard makefile variables, eg below:
#
# CC
# LINKER
# DEST
#
# this variable points to intermediate install directory during package build
# PKGBLD
#
#
# each subdirectory has its own Makefile, called from top directory (this one), eg
# cd visi; make all CC=$(CC) LINKER=$(LINKER) DEST=$(DEST) PKGBLD=../$(PKGBLKD) ...

#
# Please, first do  check all READ_ME files under sources tree.
#
# Uncomment exclusively the lines ASHLIB,CC,LINKER,CFLAGS_VISI,CFLAGS_ASH
# for the platform required
#
# make all     - compile sources & external visi pages
# make install - install binaries
# make clean   - clean all temporaries

# alpha osf1
# DEC ALPHA AXP, OSF/1 3.2 or DEC UNIX
#
#ASHLIB      = SVR4
#CFLAGS_VISI = -DUSE_TERMIOS
#CFLAGS_ASH  = -DJCLsuspend -DSYSV -DSVR4 -DMGROUPS
#CC	     = cc -std0 -g0 -O0 -taso -misalign
#CC	     = cc -std0 -g0 -O3 -taso -misalign
#LINKER      = cc -taso -misalign

# sgi
# IRIX 6.2, 6.x with IDO installed
#
##CFLAGS_VISI = -DUSE_TERMIOS -DLCASE=XCASE
##CFLAGS_ASH  = -DJCLsuspend -DSYSV -DSVR4 -DMGROUPS
##ASHLIB      = SVR4
# binaries portable to all IRIX 6.2 and later, probably to IRIX 5.3
##CC	     = cc -O -32 -cckr
##LINKER      = cc -32 -cckr
#
# binaries portable to all modern platforms, based on IRIX 6.2 and later only
#CC	     = cc -O3 -n32 -cckr -mips3
#LINKER      = cc -n32 -cckr -mips3
#
# binaries only for true 64 bit platforms, such as origin200
#CC	     = cc -O3 -64 -cckr
#LINKER      = cc -64 -cckr

# lunix
# Mudnrake, Read Hate, etc...with gcc
#
CFLAGS_VISI = -DUSE_TERMIOS
#CFLAGS_VISI = -DUSE_TERMIOS -DNOUSE_STRCHR -DNOUSE_STRCP
CFLAGS_ASH  = -DJCLsuspend -DSVR4 -DMGROUPS -DLUNIX
ASHLIB      = SVR4
TERMLIB     = -ltermcap
#TERMLIB     = -lncurses

#
#CC          = gcc -traditional -Wtraditional -Wall
#CC          = gcc -Wp,-std=c89
#CC          = gcc -traditional

# ARM arm7l
# RHEL/CentOS 7 (arm7l)
#DEBUG       =
# -g -O0 
#CC          = gcc $(DEBUG) -fno-builtin-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
#LINKER      = gcc $(DEBUG) -fno-builtin-function -static

# Intel i686/x86_64
# Fedora FC4 and later, tested up to Fedora fc23, with gcc 4.0.* and later
# no compile optimization, no dynamic libraries, 32bit compatible with x64_64 also
#DEBUG       =
# -g -O0 
#CC          = c89 -m32 $(DEBUG) -fno-builtin-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-implicit-int -Wno-int-conversion -Wno-incompatible-pointer-types
#LINKER      = c89 -m32 $(DEBUG) -fno-builtin-function -static
#
#CC          = gcc34 -m32 $(DEBUG) -fno-builtin-function -Wno-implicit-int 
#LINKER      = gcc34 -m32 $(DEBUG) -fno-builtin-function -static
# tested on gcc-5.3.1 Fedora 23
#CC          = gcc -m32 $(DEBUG) -fno-builtin-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-implicit-int -Wno-int-conversion -Wno-incompatible-pointer-types
#LINKER      = gcc -m32 $(DEBUG) -fno-builtin-function -static
#
# tested on gcc-6.1.1 Fedora 24
#DEBUG = -g
DEBUG = -g -O0
#DEBUG =
#CC          = gcc -m32 $(DEBUG) -fno-builtin-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
#
# gcc --version >= 3.x <= 6.x
#CC          = gcc $(DEBUG) -fno-builtin-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
#	-Wno-implicit-int -Wno-int-conversion -Wno-incompatible-pointer-types \
#	-Wno-implicit-function-declaration
# gcc --version <= 3.x
CC          = gcc $(DEBUG) -fno-builtin-function \
			-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
			-Wno-implicit-int \
			-Wno-implicit-function-declaration
#LINKER      = gcc -m32 $(DEBUG) -fno-builtin-function -static
LINKER      = gcc $(DEBUG) -fno-builtin-function
#
#CC          = gcc34 -m32 -fno-builtin-function
#LINKER      = gcc34 -m32 -fno-builtin-function -static
#
#CC          = gcc34 -m32 -g -O0 -fno-builtin -fno-builting-function
#LINKER      = gcc34 -m32 -g -fno-builtin -static

#tested RH 8.0:
#CC          = gcc -fwritable-strings
#LINKER      = gcc -traditional
#############
#CC	    = kgcc -fwritable-strings -static
#LINKER      = kgcc
#tested RH 7.0:
#CC	    = kgcc -fwritable-strings
#LINKER      = kgcc
#
#LINKER      = ld -Bstatic /usr/lib/crt1.o -lc
#CC	= pcc -mi386 -static -k -O0 -t
#LINKER	= pcc -static

# uw
# UnixWare 2.0.x, 1.1.x (tested)
# probably Solaris 2.x with WorkShop C (not tested)
#
#CFLAGS_VISI = -DUSE_TERMIOS
#CFLAGS_ASH  = -DJCLsuspend -DSYSV -DSVR4 -DMGROUPS
#ASHLIB      = SVR4
#CC	     = cc -O
#LINKER      = cc

# bsdi
# FreeBSD, BSDI, or similar
#
#CFLAGS_VISI = -DUSE_TERMIOS_386BSD -DNOUSE_STRCHR
#CFLAGS_VISI = -DUSE_TERMIOS_386BSD
#CFLAGS_ASH    = -DJCLsuspend -DFbsd -DBSDpagesize -DBSDmagic -DMGROUPS
#CFLAGS_ASH    = -DJCLsuspend -DFbsd -DBSDpagesize -DBSDmagic
#CFLAGS_ASH    = -DJCLsuspend -DFbsd -DBSDmagic
#ASHLIB      = BSD
#CC          = gcc -traditional
#LINKER      = gcc -traditional

# sunos
# SunOS 4.1.x
#
#CFLAGS_VISI = -DUSE_TERMIOS
#CFLAGS_ASH  = -DJCLsuspend -DBSDmagic -DMGROUPS
#ASHLIB      = BSD
#CC          = cc -O
#LINKER      = cc

# isc
# Interactive UNIX 3.0
#
#CFLAGS_VISI = -DUSE_TERMIO
#CFLAGS_ASH  = -DJCLsuspend -DSYSV
#ASHLIB      = ISC
#CC          = cc -O
#LINKER      = cc

# dynix
# Dynix/PTX 1.4.0
# NOTES: not tested yet
#
#CFLAGS_VISI = -DUSE_TERMIO
#CFLAGS_ASH  = -DJCLsuspend -DSYSV
#ASHLIB      = ISC
#CC          = cc -O -Wc,-bcs
#LINKER      = cc -Wc,-bcs

