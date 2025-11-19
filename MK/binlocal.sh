:
#!/bin/sh
#
# make binaries running without installation
#
# c&p from MK-buildrm.sh

pwd=`/bin/pwd`
conf=./BLD.cfg
prog=vash
where=..

# version of package in ./VERSION
usage() {
cat <<EOF
Usage: $0
EOF
}

version=`cat ./VERSION`
pkgname="$prog-$version-local"

# this script needs compiled binaries
### make distclean
#set -x

if [ ! -d ./BLD ]; then
    echo " no binaries found in '.'"
    exit 1
fi

# pkgname actually is symlink to sources dir
if [ -s $where/$pkgname ]; then
    rm -f $where/$pkgname
fi
ln -s $pwd $where/$pkgname \
&& ( cd $where && /bin/pwd \
&& tar czf "$pkgname.tar.gz" \
            $pkgname/BLD \
            $pkgname/etc \
            $pkgname/bin \
            $pkgname/v \
) \
&& rm -f $where/$pkgname
