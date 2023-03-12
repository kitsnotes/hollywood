#!/bin/bash

# Script to build a Hollywood Universal ISO

HWARM64="/home/buildbot/HWARM64.ISO"
HWAMD64="/home/buildbot/HWAMD64.ISO"
OUTPUT="/home/buildbot/HWUNIV.ISO"

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

WORKDIR=/tmp/hw-iso-merge
ISONAME=HWUNIV
ARMNAME=HWARM64
AMDNAME=HWAMD64

mkdir -p "$WORKDIR"/amd64
mkdir -p "$WORKDIR"/arm64
mkdir -p "$WORKDIR"/output

# sync our iso's into one
mount -o loop "$HWARM64" "$WORKDIR"/arm64
mount -o loop "$HWAMD64" "$WORKDIR"/amd64
rsync -a "$WORKDIR"/arm64/ "$WORKDIR"/output/
rsync -a "$WORKDIR"/amd64/ "$WORKDIR"/output/
ETAMD=$(du -s "$WORKDIR"/amd64/efi.img | cut -f 1)
ETARM=$(du -s "$WORKDIR"/arm64/efi.img | cut -f 1)
NEWEFI=0
((NEWEFI=ETAMD+ETARM))
umount "$WORKDIR"/arm64
umount "$WORKDIR"/amd64
rmdir "$WORKDIR"/arm64
rmdir "$WORKDIR"/amd64

# correct refind.conf variables with new disk label
rm "$WORKDIR"/output/efi.img
sed "s/$AMDNAME/$ISONAME/" \
	"$WORKDIR"/output/EFI/BOOT/refindx64.conf \
	> "$WORKDIR"/output/EFI/BOOT/refindx64.conf_new

sed "s/$AMDNAME/$ISONAME/" \
        "$WORKDIR"/output/System/Library/CoreServices/refindx64.conf \
        > "$WORKDIR"/output/System/Library/CoreServices/refindx64.conf_new

sed "s/$ARMNAME/$ISONAME/" \
        "$WORKDIR"/output/EFI/BOOT/refindaa64.conf \
        > "$WORKDIR"/output/EFI/BOOT/refindaa64.conf_new

mv "$WORKDIR"/output/EFI/BOOT/refindx64.conf_new "$WORKDIR"/output/EFI/BOOT/refindx64.conf
mv "$WORKDIR"/output/EFI/BOOT/refindaa64.conf_new "$WORKDIR"/output/EFI/BOOT/refindaa64.conf
mv "$WORKDIR"/output/System/Library/CoreServices/refindx64.conf_new "$WORKDIR"/output/System/Library/CoreServices/refindx64.conf
# re-build our efi.img
cat >"$WORKDIR"/mtoolsrc <<-MTOOLSRC
        drive A: file="$WORKDIR/output/efi.img"
MTOOLSRC
export MTOOLSRC="$WORKDIR"/mtoolsrc
dd if=/dev/zero of="$WORKDIR/output/efi.img" bs=1024 count=$NEWEFI
mkfs.fat "$WORKDIR/output/efi.img"
mmd A:/boot

mcopy -s "$WORKDIR"/output/boot/dtbs-stable/ A:/boot/
mcopy -s "$WORKDIR"/output/boot/dtbs-asahi/ A:/boot/
mcopy "$WORKDIR"/output/boot/vmlinuz-stable.amd64 A:/boot/
mcopy "$WORKDIR"/output/boot/initramfs-stable.amd64 A:/boot/
mcopy "$WORKDIR"/output/boot/vmlinuz-stable.arm64 A:/boot/
mcopy "$WORKDIR"/output/boot/initramfs-stable.arm64 A:/boot/
mcopy "$WORKDIR"/output/boot/vmlinuz-asahi.arm64 A:/boot/
mcopy "$WORKDIR"/output/boot/initramfs-asahi.arm64 A:/boot/
mcopy -s "$WORKDIR"/output/EFI/ A:/

# Edit our autorun.ini to reflect a new label
sed "s/label=.*/label=Hollywood Universal/g" "$WORKDIR/output/autorun.inf" > "$WORKDIR"/output/autorun2.inf
mv "$WORKDIR"/output/autorun2.inf "$WORKDIR"/output/autorun.inf

# Create our new ISO
xorriso -as mkisofs -o "$WORKDIR"/$ISONAME.ISO -joliet -rational-rock -V $ISONAME -efi-boot-part --efi-boot-image -e efi.img -no-emul-boot "$WORKDIR"/output/

# cleanup
rm -rf "$WORKDIR"/output
rm "$WORKDIR"/mtoolsrc
mv "$WORKDIR"/$ISONAME.ISO "$OUTPUT"