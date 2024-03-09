:
#!/bin/sh
#
# make source RPM package
#
# make distclean
# find spec
# build source archive tar.gz
# place archive and spec files to rpmbuild tree
# build src.rpm in ~/rpmbuild/SRPMS
#
rpmbuild="$HOME/rpmbuild"
mach=$(uname -m)
SPECS="$rpmbuild/SPECS"
SOURCES="$rpmbuild/SOURCES"
RPMS="$rpmbuild/RPMS"

cd `/bin/pwd`
pwd=$(pwd)

conf=./BLD.cfg
prog=vash
where=.
where=..

# version of package in ./VERSION
usage() {
cat <<EOF
Usage: $0
EOF
}

spec=
case $# in
    0) spec="$prog.spec"
    ;;
    1) spec="$prog-$1.spec"
    ;;
    *) usage; exit 1
    ;;
esac

version=`cat ./VERSION`
pkgname="$prog-$version"

make distclean
#set -x

# pkgname actually is symlink to sources dir
if [ -s $where/$pkgname ]; then
    rm -f $where/$pkgname
fi
ln -s $pwd $where/$pkgname && \
( cd $where && tar czf "$pkgname.tar.gz" $pkgname/* ) \
&& rm -f $where/$pkgname


# make a spec from template then build src.rpm

# prepare spec from template
#src_specfile=$(find $where -name $spec)
src_specfile=$(find . -name $spec)
echo src_specfile=$src_specfile
#
specfile=$(basename $src_specfile)
#
rm -f $SPECS/$specfile || exit
cat $src_specfile | sed "s/^Version:.*\$/Version:    $version/g" \
> $SPECS/$specfile || exit

##tar czf "$pkgname.tar.gz" $pkgname/*

ln -sf "$pwd/$where/$pkgname.tar.gz" $SOURCES || exit

cd $rpmbuild && rpmbuild -bs $SPECS/$specfile
