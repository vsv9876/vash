#!/bin/sh

#
# dirty hack for Debian9 and probably for any debian-compatible -
# only fakeroot and dpkg requred

BLDCFG=BLD.cfg
BUILD=`pwd`/BLD
arch=`dpkg --print-architecture`

#make distclean
#./configure
make
if [ $? -ne 0 ]; then
    echo STOP...; exit 1
fi
make DESTDIR=${BUILD} install

# install configs
cp -rp etc ${BUILD}

# install docs and manuals
mkdir -p 	 ${BUILD}/usr/share/doc/vash
cp -rp ./LICENSE ${BUILD}/usr/share/doc/vash
mkdir -p 	${BUILD}/usr/share/man/man1
cp -rp ./vash.1 ${BUILD}/usr/share/man/man1
gzip 		${BUILD}/usr/share/man/man1/*

#cp -rp DEBIAN/ BLD
set -x
#VERSNSH=`cat $BLDCFG| grep ^VERSN | sed -e 's/ //g' -e 's/-/ /'`
#eval $VERSNSH
VERSN=`cat ./VERSION | sed -e 's/ //g' -e 's/-/ /'`
echo $VERSN
read pkg ver <<EOF
$VERSN
EOF

mkdir BLD/DEBIAN
cat DEBIAN/control \
    | sed -e 's/Architecture:.*$/Architecture: '${arch}/ \
    | sed -e 's/Version:.*$/Version: '${ver}/ \
    > BLD/DEBIAN/control

fakeroot dpkg -b BLD

mv BLD.deb ../${pkg}_${ver}_${arch}.deb

