:
#
# Assistant for sh startup
# Version for SVR4.x
#------------------------------
# First Bourne' shell started,
# then exec setup environ, then exec ash
#------------------------------
#
# DEBUG switch: uncomment next line.
# set -x

TERMCAP=/usr/local/etc/termcap
export TERMCAP

echo '
		To use on-line help type ? or HELP metakey

	Assistant Shell for UNIX.
	(C) by Sergey V.Vovk, 1990-1996.
	All Rights Reserved.
'
#
#---------------------------------------------
# environ-passed setup (ASH):
#
# h     history will be stored
# lNN   lines for filename menu (from 2 to 20), default 10
# p     key help panel bar on
# s     scroll cmd script but no clear screen
# c     clock show,  uprigh corner of screen
# m     check mailbox (Biff)
#
# example for assist environ-passed setup:

ASH=l10pshcm

#---------------------------------------------
# label for identification machine name, /dev/tty???,
# and other likely info.
# ASHLBL
#
case l"$LOGNAME" in
l)      case u"$USER" in
	u)      user=''
		;;
	u*)     user="$USER"
		;;
	esac
	;;
l*)     user="$LOGNAME"
	;;
esac

ttyfull=`tty`
tty=`basename $ttyfull`
uname=`uname -n || hostname | sed -e 's/\..$*//' 2>/dev/null`
ASHLBL=' '"$uname:$tty"' '"$user"' '

case $PATH in
*/usr/local/bin*)     ;;
*)
	PATH=$PATH:/usr/local/bin
esac

# .ashstd: #e - EDITOR, #m - PAGER
#EDITOR=re
#PAGER=/usr/local/bin/m

export ASHLBL PATH
#export EDITOR PAGER

#stty kill '^u'

# decoration on xterm window
case $TERM in
xterm*|vs100*|iris-ansi)
	# reset PS1 
	PS1='.ash> '; export PS1
	ASH=l10sh
	echo ']0;'"$ASHLBL"''
	#eval `/usr/bin/X11/resize -u`
	eval `/usr/local/bin/resz`
	export TERMCAP
	;;
esac

export ASH
exec /usr/local/bin/ash $@
#exec ash $@
