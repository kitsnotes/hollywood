#!/bin/sh

pkgout="/home/buildbot/packages.txt"
scriptdir="$(dirname "$0")"
git=$(command -v git) || git=true

# deduce aports directory
[ -n "$APORTS" ] || APORTS=$(realpath "$scriptdir/")
[ -e "$APORTS/system/build-base" ] || die "Unable to deduce aports base checkout"

while read pkg; do
	cd /home/buildbot/hollywood-packages/system/$pkg
	abuild -r
	cd ../
done < $pkgout
