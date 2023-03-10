#!/bin/sh
#
# Visual Assistant for shell startup;
# Version for GNU/Linux (like SVR4.x)

# set -x;# debug this script, if uncomment
#-------
# compatible locale setup for old 8-byte cyrillic encoding,
# only 2 categories needed: CTYPE and COLLATE
#export LC_ALL=
#export LANG=en_US.ISO8859-1
#export LC_CTYPE=ru_RU.KOI8-R
#export LC_COLLATE=$LC_CTYPE

#-------
# Please, for full description of environ-based setup parameters
# refer to man vash.1
# also available via online setup page (on key 9 or F9)
#
# there some example:
#
# h     history will be saved in file
# S     history will be synced, or saved on exit if not specified
# p     key help panel bar on
# lNN   lines for filename menu from "2" to "maxlines-2",
#       default 10 if not specified
#
#VASH=l10pshcm
#VASH=

# library stuff of vash:
#VASH_EXDIR=/usr/lib/vash
#debug:
#VASH_EXDIR=$HOME/proj-ws/remix/vashlib/LIB
#VASH_PATH=/home/user/.vash:/usr/lib/vash

#-------
# setup generic usage look:
# VASH_LABEL:
#   label for identification - tty, whoami, hostname
# hint, since vash-1.26.4:
# if last symbol in VASH_LABEL is ':' (colon),
# vash will append current working directory

if [ x"${VASH_LABEL}" = x ]; then
    case l"$LOGNAME" in
    l)  case u"$USER" in
    	u)      user='' ;;
    	u*)     user="$USER" ;;
    	esac
    	;;
    l*)     user="$LOGNAME"
    	;;
    esac
    ttyfull=`tty`
    tty=`echo $ttyfull|sed -e 's-/dev/--'`
    uname=`uname -n || hostname | sed -e 's/\..$*//' 2>/dev/null`
    VASH_LABEL="$tty $user@$uname:"
    export VASH_LABEL
fi

# variables, supported via program files, like .ashstd:
# substituted in .ashstd:
# #e - EDITOR, #m - PAGER
if [ x${EDITOR} = x ]; then
    EDITOR=vi; export EDITOR
fi

if [ x${PAGER} = x ]; then
    #PAGER=/usr/local/bin/m $ script obsoleted
    PAGER=less
    export PAGER
    LESS=-qmeXR
    export LESS
fi
# make ~/.bashrc aliases workable
# (in extern shell only via setup mode
#  or string terminated with ';')

# make sure your .bashrc contains option: 
#	shopt -s expand_aliases
# https://emacs.stackexchange.com/questions/3447/cannot-set-terminal-process-group-error-when-running-bash-script

if [ x"$SHELL" != x -a x`basename $SHELL` = xbash ]; then
	BASH_ENV="$HOME/.bashrc"
	export BASH_ENV
fi

#stty kill '^u'

PS1='v> '; export PS1

if [ x${VASH} = x ]; then
    #VASH=l10shwST
    VASH=l8shwS

    # decoration on xterm window
    onxterm=0
    if [ x$DISPLAY != x ]; then
        onxterm=1
    fi
    case $TERM in
    xterm*|vs100*|iris-*)
        onxterm=1;;
    screen*|tmux*)
        onxterm=0;;
    esac
    if [ 1 -eq $onxterm ]; then
    	# X11 window decoration on xterm 
    	# now compiled into vash binary:
    	VASH=px$VASH
    else
        VASH=p$VASH
    fi
    # append clock on hardware console screen
    case $TERM in
    console|linux|pc*|vs100*|AT386)
    	VASH=c$VASH
    esac

    export VASH
fi

exec vash "$@"
