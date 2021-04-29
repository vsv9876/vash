#!/bin/sh

# vash -- Visual Assistant Shell
# src directory
vashdir="$HOME/proj-ws/vash-2."
#bintest=vash/vash
bintest=.

# debug env, reset my own regular setup
if [ -x $vashdir/$bintest ]; then
    onpath=`echo $PATH|grep -c $vashdir/vash:`
    if [ $onpath -eq 0 ]; then
	PATH=$vashdir/vash:$PATH;
	PATH=$vashdir/visi/bin:$PATH
	PATH=$vashdir/vhset:$PATH
	export PATH
    fi
    VASH_EXDIR=$vashdir/vashlib/LIB
    export VASH_EXDIR
    VASH_PATH=$VASH_EXDIR:/usr/lib/vash
    export VASH_PATH
    VHSET=$vashdir/etc/vhset
    export VHSET
    VASH_LABEL='debug:'
    export VASH_LABEL
    echo ' ***'
    echo " *** Warning: this is DEBUG environment for $vashdir ***"
    echo ' ***'
fi
exec /usr/bin/v "$@"
