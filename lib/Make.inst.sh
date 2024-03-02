:

#
# install into directory which specified with 1st arg
# all remain modules
#
# base name may be directory or rc-file,
# so check it first
#

if [ $# -le 1 ]
then
echo please, use syntax: ./`basename $0` dest_dir module ...
exit 1
fi

dest="$1"
shift

_echo=
# uncomment, if debug required
#_echo=echo

for module
do
    for base in "${module}" "${module}".*
    do
	if test -d "$base" ; then
	    $_echo cp -t $dest -r $base
	elif test -f "$base" ; then
	    $_echo cp -t $dest $base
	fi
    done
done
