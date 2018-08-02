#!/bin/sh

# dirty hack for Debian9

make distclean
./configure
make
make DESTDIR=`pwd`/BLD install
cp -rp DEBIAN/ BLD
fakeroot dpkg -b BLD
mv BLD.deb vash_1.23.1_amd64.deb


