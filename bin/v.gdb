#!/bin/sh

# vash -- Visual Assistant Shell

# startup script for DEBUG unpackaged vash
# copy&paste from v.local, gdb as a prefix

# name of this script as regular file
cmd=v.gdb
#
# recognize src directory:
dn=`dirname $0`
l=`ls -ld $0`
case $l in
l*)	;;
*)	echo $0 is not a symlink to $cmd
        exit 1 ;;
esac
d=`echo $l|sed 's%^.*->%%'`
d=`echo $d`
case $d in
    /*) 	ddd=$d;;
  *|../*)	ddd=`dirname $dn/$d`;
esac
#
ddd=`dirname $ddd`
if [ -s $ddd/BLD.cfg ]; then
    s=`grep TOPDIR $ddd/BLD.cfg`
    IFS== read k v<< EOF
    $s
EOF
    vashdir=`echo $v`
    ##exit 2
else
    case $ddd in
	/*) vashdir=$ddd  ;;
	.)  vashdir=`pwd` ;;
	*)  vashdir=`pwd`/$ddd ;;
    esac
fi

# where 'make install' done
BLD=$vashdir/BLD
#bintest=vash/vash
bintest=$BLD/usr/bin/vash

# debug env, reset my own regular setup
if [ -x $bintest ]; then
    onpath=`echo $PATH|grep -c $BLD/usr/bin`
    if [ $onpath -eq 0 ]; then
	PATH=$BLD/usr/bin:$PATH;
	export PATH
    fi
    VASH_PATH="${BLD}/usr/lib/vash:/usr/lib/vash"
    export VASH_PATH
    VHSET="${vashdir}/etc/vhset"
    export VHSET
    VHSET_LIB="${BLD}/usr/lib/vhset:/usr/lib/vhset"
    export VHSET_LIB
    echo ' *'
    echo " * Warning: this is LOCAL (unpackaged) version "
    echo " * from $vashdir ***"
    echo ' *'
    exec gdb $BLD/usr/bin/vash "$@"
fi
echo '   *** no compiled binaries found in:'
echo '          "'$BLD'"'
echo ''
echo '   *** please,'
echo '       cd '$vashdir
echo '       make clean install'
exit 1
