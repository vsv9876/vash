#!/bin/sh

# dirty hack for Debian9

make cleandist
./configure
make compile
make DESTDIR=`pwd`/BLD install
cp -rp DEBIAN/ BLD
fakeroot dpkg -b BLD
mv BLD.deb vash_1.22.3_amd64.deb


