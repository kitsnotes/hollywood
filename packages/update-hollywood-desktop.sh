#!/bin/bash

SSHHOST="10.99.98.9"

# Keep this in sync with the APKBUILDS that pull from hollywood-desktop git
declare -a updatepkgs=("hollywood-compositor"
			"hollywood-qt5-libs"
			"hollywood-qt5-libshell"
			"hollywood-qt6-libs"
			"hollywood-settings"
			"hollywood-shell"
			"hollywood-sysmon"
			"terminull"
			)

scriptdir="$(dirname "$0")"
git=$(command -v git) || git=true

# deduce aports directory
[ -n "$SRCDIR" ] || SRCDIR=$(realpath "$scriptdir/")
[ -e "$SRCDIR/system/build-base" ] || die "Unable to deduce aports base checkout"

doas rm -f /var/cache/distfiles/hollywood-desktop-1.0.tar.gz
ssh cat@$SSHHOST hollywood-git-refresh.sh
for i in "${updatepkgs[@]}"
do
   echo "$i"
   cd "$SRCDIR/system/$i"
   apkgrel -a .
   abuild checksum
   git add $SRCDIR/system/$i/APKBUILD
   git commit -m "Automated update of package $i for hollywood git refresh"
   cd $SRCDIR
done
