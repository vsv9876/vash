:
#!/bin/sh
#
# make FreeBSD binary package
#

pwd=`/bin/pwd`
conf=./BLD.cfg
distdir=./BLD
MANIFEST=.manifest
PLIST=.plist
arch=`uname -m`

prog=vash
where=..

# version of package in ./VERSION
version=`cat ./VERSION`
pkgname="$prog-$version"

# this script needs compiled binaries
if [ ! -d ./BLD ]; then
    echo "    no binaries found in '$distdir'"
    echo "Please, make config install"
    exit 1
fi

# vhset config files not covered by build process...
mkdir -p $distdir/etc
cp -RHpf etc/ $distdir/etc/

#flatsize=`du -sh $distdir|awk '{print $1}'`

# generate manifest, plist, pkg

#flatsize: $flatsize
cat > $MANIFEST <<EOM
name: $prog
version: $version
origin: shell/vash
comment: Visual Assistant Shell
desc: Visual Assistant Shell
www:
arch: $arch
prefix: /usr/local
maintainer: vsv
EOM
#exit

find BLD -type f | sed -e 's/^BLD//' > $PLIST
find BLD -type l | sed -e 's/^BLD//' >> $PLIST

pkg create -o .. -r BLD/. -p $PLIST -M $MANIFEST 

#clean tmp files
rm -rf $PLIST $MANIFEST
