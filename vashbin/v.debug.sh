#!/bin/sh

# vash -- Visual Assistant Shell
# src directory
vashdir="$HOME/proj-ws/vash-2."
# where 'make install' done
BLD=$vashdir/BLD
#bintest=vash/vash
bintest=$BLD/usr/bin/vash

# debug env, reset my own regular setup
if [ -x $bintest ]; then
    onpath=`echo $PATH|grep -c $vashdir/vash:`
    if [ $onpath -eq 0 ]; then
	PATH=$vashdir/vash:$PATH;
	PATH=$vashdir/visi/bin:$PATH
	PATH=$vashdir/vhset:$PATH
	export PATH
    fi
    VASH_EXDIR="${BLD}/usr/lib/vash:/usr/lib/vash"
    export VASH_EXDIR
    VASH_PATH="${BLD}/usr/lib/vash:/usr/lib/vash"
    export VASH_PATH
    VHSET="${vashdir}/etc/vhset"
    export VHSET
    VHSET_LIB="${BLD}/usr/lib/vhset:/usr/lib/vhset"
    export VHSET
    VASH_LABEL='debug:'
    export VASH_LABEL
    echo ' ***'
    echo " *** Warning: this is DEBUG environment "
    echo " *** for $vashdir ***"
    echo ' ***'
    exec $BLD/usr/bin/v
fi
echo '   *** no subdirectory "'$BLD'" found'
echo '   *** please, "make clean install"'
exit 1
#exec /usr/bin/v "$@"
