#!/bin/sh

# dirty hack for Debian9

make cleandist
./configure
make complile
make DESTDIR=`pwd`/BLD install
cp -rp DEBIAN/ BLD
fakeroot dpkg -b BLD
mv BLD.deb vash_1.19_amd64.deb


