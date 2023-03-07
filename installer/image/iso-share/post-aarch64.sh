#!/bin/sh

mkdir -p cdroot/boot/EFI/BOOT/
mkdir -p cdroot/boot/EFI/BOOT/drivers/
mkdir -p cdroot/boot/EFI/BOOT/icons/

cp /usr/share/refind/refind_aa64.efi cdroot/boot/EFI/BOOT/BOOTAA64.EFI
cp /usr/share/refind/drivers_aa64/* cdroot/boot/EFI/BOOT/drivers/
cp /usr/share/refind/icons/*.png cdroot/boot/EFI/BOOT/icons/

cat >cdroot/boot/EFI/BOOT/refind.conf <<'REFINDCFG'
timeout 10
log_level 0
use_graphics_for linux, windows, osx
showtools shell, memtest, firmware
scanfor manual
scan_delay 0
dont_scan_volumes "Recovery HD"
fold_linux_kernels true
hideui editor badges hints

menuentry "Hollywood Live (For all other systems)" {
        icon EFI/BOOT/icons/hw_install.png
        volume HWARM64
        loader /boot/kernel-stable
        initrd /boot/initrd-stable
        options "root=live:LABEL=HWARM64 rd.live.dir=/ rd.live.squashimg=aarch64.squashfs quiet splash"
}

menuentry "Hollywood Live (For Apple Silicon)" {
        icon EFI/BOOT/icons/hw_install_asi.png
        volume HWARM64
        loader /boot/kernel-asahi
        initrd /boot/initrd-asahi
        options "root=live:LABEL=HWARM64 rd.live.dir=/ rd.live.squashimg=aarch64.squashfs quiet splash"
}

REFINDCFG

# Get the size of the binaries to go in the El Torito image in kB
ToritoSize=$(du -s cdroot/boot/ | cut -f 1)
((ToritoSize=ToritoSize/28))
((ToritoSize=ToritoSize*32))

mv cdroot/boot/EFI cdroot/EFI

if ! type mkfs.fat>/dev/null || ! type mtools>/dev/null; then
        printf "EFI image cannot be created.\n"
        printf "Install dosfstools and mtools.\n"
else
        cat >mtoolsrc <<-MTOOLSRC
        drive A: file="efi64.img"
MTOOLSRC
        export MTOOLSRC="$PWD/mtoolsrc"
        dd if=/dev/zero of=efi64.img bs=1024 count=$ToritoSize
        mkfs.fat efi64.img
        mcopy -s cdroot/boot/ A:/
        mcopy -s cdroot/EFI/ A:/
        mv efi64.img cdroot/efi.img
fi