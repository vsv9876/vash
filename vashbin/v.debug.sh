#!/bin/sh

# vash -- Visual Assistant Shell
#
# recognize src directory:
#vashdir="$HOME/proj-ws/vash-2."
dn=`dirname $0`
l=`ls -ld $0`
case $l in
l*)	;;
*)	echo $0 not a symlink; exit 1 ;;
esac
d=`echo $l|sed 's%^.*->%%'`
d=`echo $d`
case $d in
/*) 	ddd=$d;;
../*)	ddd=`dirname $dn/$d`;
esac
ddd=`dirname $ddd`
#ddd=`dirname $ddd`
s=`grep TOPDIR $ddd/BLD.cfg`
IFS== read k v<< EOF
$s
EOF
vashdir=`echo $v`
##exit 2
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
echo '   *** no subdirectory found:'
echo '          "'$BLD'"'
echo ''
echo '   *** please, "cd '$vashdir' && make clean install"'
exit 1
#exec /usr/bin/v "$@"
