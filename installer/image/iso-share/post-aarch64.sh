#!/bin/sh

mkdir -p cdroot/boot/
mkdir -p cdroot/EFI/BOOT/
mkdir -p cdroot/EFI/BOOT/drivers_aa64/
mkdir -p cdroot/EFI/BOOT/icons/

cp /usr/share/refind/refind_aa64.efi cdroot/EFI/BOOT/BOOTAA64.EFI
cp /usr/share/refind/drivers_aa64/* cdroot/EFI/BOOT/drivers_aa64/
cp /usr/share/refind/icons/*.png cdroot/EFI/BOOT/icons/

cat >cdroot/EFI/BOOT/refindaa64.conf <<REFINDCFG
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
        loader /boot/vmlinuz-stable.arm64
        initrd /boot/initramfs-stable.arm64
        options "root=live:LABEL=HWARM64 rd.live.dir=/boot rd.live.squashimg=live-image.arm64 quiet splash"
}

menuentry "Hollywood Live (For Apple Silicon)" {
        icon EFI/BOOT/icons/hw_install_asi.png
        volume HWARM64
        loader /boot/vmlinuz-asahi.arm64
        initrd /boot/initramfs-asahi.arm64
        options "root=live:LABEL=HWARM64 rd.live.dir=/boot rd.live.squashimg=live-image.arm64 quiet splash"
}

REFINDCFG

# Get the size of the binaries to go in the El Torito image in kB
EFIToritoSize=$(du -s cdroot/EFI/ | cut -f 1)
KernelToritoSize=$(du -s cdroot/boot/vmlinuz-stable.arm64 | cut -f 1)
InitrdToritoSize=$(du -s cdroot/boot/initramfs-stable.arm64 | cut -f 1)
KernelAToritoSize=$(du -s cdroot/boot/vmlinuz-asahi.arm64 | cut -f 1)
InitrdAToritoSize=$(du -s cdroot/boot/initramfs-asahi.arm64 | cut -f 1)
DTBSize=$(du -s cdroot/boot/dtbs-stable/ | cut -f 1)
DTBASize=$(du -s cdroot/boot/dtbs-asahi/ | cut -f 1)
ToritoSize=0
((ToritoSize=EFIToritoSize+KernelToritoSize+InitrdToritoSize+KernelAToritoSize+InitrdAToritoSize+DTBSize+DTBASize))
((ToritoSize=ToritoSize/28))
((ToritoSize=ToritoSize*32))

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
        mcopy cdroot/boot/vmlinuz-stable.arm64 A:/boot/
        mcopy cdroot/boot/initramfs-stable.arm64 A:/boot/
        mcopy cdroot/boot/vmlinuz-asahi.arm64 A:/boot/
        mcopy cdroot/boot/initramfs-asahi.arm64 A:/boot/
        mcopy -s cdroot/EFI/ A:/
        mv efi64.img cdroot/efi.img
fi
