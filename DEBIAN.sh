#!/bin/sh

make cleandist
make cfg
make bld
make DESTDIR=`pwd`/BLD install
cp -rp DEBIAN/ BLD
fakeroot dpkg -b BLD
mv BLD.deb vash_1.18_amd64.deb


