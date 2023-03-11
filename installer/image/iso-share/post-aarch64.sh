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
        loader /boot/vmlinuz-stable.aarch64
        initrd /boot/initramfs-stable.aarch64
        options "root=live:LABEL=HWARM64 rd.live.dir=/boot rd.live.squashimg=live-image.arm64 quiet splash"
}

menuentry "Hollywood Live (For Apple Silicon)" {
        icon EFI/BOOT/icons/hw_install_asi.png
        volume HWARM64
        loader /boot/vmlinuz-asahi.aarch64
        initrd /boot/initramfs-asahi.aarch64
        options "root=live:LABEL=HWARM64 rd.live.dir=/boot rd.live.squashimg=live-image.arm64 quiet splash"
}

REFINDCFG

# Get the size of the binaries to go in the El Torito image in kB
ToritoSize=$(du -s cdroot/boot/ | cut -f 1)
((ToritoSize=ToritoSize/28))
((ToritoSize=ToritoSize*32))

# Get the size of the binaries to go in the El Torito image in kB
EFIToritoSize=$(du -s cdroot/EFI/ | cut -f 1)
KernelToritoSize=$(du -s cdroot/boot/kernel* | cut -f 1)
InitrdToritoSize=$(du -s cdroot/boot/initrd* | cut -f 1)
DTBSize=$(du -s cdroot/boot/dtbs* | cut -f 1)
ToritoSize=0
((ToritoSize=EFIToritoSize+KernelToritoSize+InitrdToritoSize+DTBSize))
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
        mmd A:/boot
        mcopy -s cdroot/boot/dtbs-stable/ A:/boot/
        mcopy -s cdroot/boot/dtbs-asahi/ A:/boot/
        mcopy cdroot/boot/kernel-stable.amd64 A:/boot/
        mcopy cdroot/boot/initrd-stable.amd64 A:/boot/
        mcopy -s cdroot/EFI/ A:/
        mv efi64.img cdroot/efi.img
fi