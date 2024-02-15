#!/bin/sh

# vash -- Visual Assistant Shell

# name of this script as regular file
cmd=v.debug.sh
#
# recognize src directory:
#vashdir="$HOME/proj-ws/vash-2."
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
    ../*)	ddd=`dirname $dn/$d`;
esac
ddd=`dirname $ddd`
#ddd=`dirname $ddd`
if [ -s $ddd/BLD.cfg ]; then
    s=`grep TOPDIR $ddd/BLD.cfg`
    IFS== read k v<< EOF
    $s
EOF
    vashdir=`echo $v`
    ##exit 2
else
    vashdir=$ddd
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
    VASH_EXDIR="${BLD}/usr/lib/vash:/usr/lib/vash"
    export VASH_EXDIR
    VASH_PATH="${BLD}/usr/lib/vash:/usr/lib/vash"
    export VASH_PATH
    VHSET="${vashdir}/etc/vhset"
    export VHSET
    VHSET_LIB="${BLD}/usr/lib/vhset:/usr/lib/vhset"
    export VHSET_LIB
    VASH_LABEL='debug:'
    export VASH_LABEL
    echo ' ***'
    echo " *** Warning: this is DEBUG environment "
    echo " *** for $vashdir ***"
    echo ' ***'
    exec $BLD/usr/bin/v
fi
echo '   *** no build directory found:'
echo '          "'$BLD'"'
echo ''
echo '   *** please, "cd '$vashdir' && make config clean install"'
exit 1
#exec /usr/bin/v "$@"
