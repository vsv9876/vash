#!/bin/sh

# vash -- Visual Assistant Shell
# src directory
vashdir="$HOME/proj-ws/vash-1."

# debug env, reset my own regular setup
if [ -x $vashdir/vash/vash ]; then
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
    ASHLBL='debug:'
    export ASHLBL
    echo ''
    echo "		*** Warning: this is DEBUG version of vash ***"
    echo ''
fi
exec /usr/bin/v "$@"
