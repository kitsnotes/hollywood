#!/bin/sh

pkgout="/tmp/.hollywood-build-packages.txt"
scriptdir="$(dirname "$0")"
git=$(command -v git) || git=true

# deduce aports directory
[ -n "$APORTS" ] || APORTS=$(realpath "$scriptdir/")
[ -e "$APORTS/system/build-base" ] || die "Unable to deduce aports base checkout"

rm $pkgout
find $APORTS/system -maxdepth 1 -type d | sort  > $pkgout

while read pkg; do
	cd $pkg
	abuild -r
	cd ../
done < $pkgout
rm $pkgout
