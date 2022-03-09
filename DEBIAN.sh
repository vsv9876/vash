#!/bin/sh

#
# dirty hack for Debian9 and probably for any debian-compatible -
# only fakeroot and dpkg requred

BLDCFG=BLD.cfg

arch=`dpkg --print-architecture`

make distclean
./configure
make
make DESTDIR=`pwd`/BLD install

# install configs
cp -rp etc `pwd`/BLD

#cp -rp DEBIAN/ BLD
set -x
VERSNSH=`grep VERSN < $BLDCFG | sed -e 's/ //g' -e 's/-/ /'`
eval $VERSNSH
echo $VERSN
read pkg ver <<EOF
$VERSN
EOF

mkdir BLD/DEBIAN
cat DEBIAN/control |\
	sed -e 's/Architecture:.*$/Architecture: '"$arch/" \
    -e 's/Version:.*$/Version: '"$ver/"	> BLD/DEBIAN/control

fakeroot dpkg -b BLD
mv BLD.deb ../vash_${ver}_${arch}.deb
