#!/bin/sh

mkdir -p cdroot/boot

cat >early.cfg <<'EARLYCFG'
search.fs_label "HWARM64" root
set prefix=($root)/boot
EARLYCFG

cat >cdroot/boot/grub.cfg <<'GRUBCFG'
load_video
set gfxpayload=keep
insmod gzio
insmod part_gpt
insmod ext2
insmod iso9660
insmod linux
search --label "HWARM64" --no-floppy --set
set timeout=5

menuentry "Hollywood (ARM 64-bit - for all VMs and machines not listed below)" --class linux --id hollywood-stable {
        linux ($root)/boot/kernel-stable root=live:LABEL=HWARM64 rd.live.dir=/ rd.live.squashimg=aarch64.squashfs quiet splash
        initrd ($root)/boot/initrd-stable
}

menuentry "Hollywood (Apple Silicon - on bare metal only)" --class linux --id hollywood-asahi {
        linux ($root)/boot/kernel-asahi root=live:LABEL=HWARM64 rd.live.dir=/ rd.live.squashimg=aarch64.squashfs quiet splash
        initrd ($root)/boot/initrd-asahi
}

GRUB_DEFAULT=hollywood-stable
GRUB_DISABLE_OS_PROBER=true
GRUB_TIMEOUT=5
GRUB_DISTRIBUTOR="Hollywood"
GRUBCFG

if ! type grub-mkimage>/dev/null; then
	printf "GRUB image cannot be created.\n"
	exit 1
else
	printf '\033[01;32m * \033[37mInstalling GRUB...\033[00;39m\n'
        grub-mkimage -d target/usr/lib/grub/arm64-efi -c early.cfg -p boot -o efi64.exe -O arm64-efi boot btrfs datetime disk ext2 gfxmenu help iso9660 ls luks lvm memdisk nilfs2 normal part_gpt part_msdos png scsi search fat linux reboot gfxterm gfxterm_background gfxterm_menu all_video
fi

rm early.cfg

if ! type mkfs.fat>/dev/null || ! type mtools>/dev/null; then
	printf "EFI image cannot be created.\n"
	printf "Install dosfstools and mtools.\n"
else
	cat >mtoolsrc <<-MTOOLSRC
	drive A: file="efi64.img"
	MTOOLSRC
	export MTOOLSRC="$PWD/mtoolsrc"
	dd if=/dev/zero of=efi64.img bs=1024 count=1440
	mkfs.fat efi64.img
	mmd A:/EFI
	mmd A:/EFI/BOOT
	mcopy efi64.exe A:/EFI/BOOT/bootaa64.efi
	rm efi64.exe mtoolsrc
	mv efi64.img cdroot/efi.img
fi
