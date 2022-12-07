#!/bin/sh

mkdir -p cdroot/boot

cat >early.cfg <<'EARLYCFG'
search.fs_label "HWAMD64" root
set prefix=($root)/boot
EARLYCFG

cat >cdroot/boot/grub.cfg <<'GRUBCFG'
set timeout=0
menuentry "Hollywood (AMD/Intel 64-bit)" --class linux --id hollywood-stable {
        insmod iso9660
        insmod linux
        search --label "HWAMD64" --no-floppy --set
        linux ($root)/boot/kernel-stable-x86_64 root=live:LABEL=HWAMD64 rd.live.dir=/ rd.live.squashimg=x86_64.squashfs quiet splash
        initrd ($root)/boot/initrd-stable-x86_64
}

GRUB_DEFAULT=hollywood-stable
GRUB_DISABLE_OS_PROBER=true
GRUB_TIMEOUT=0
GRUB_DISTRIBUTOR="Hollywood"
GRUBCFG

if ! type grub-mkimage>/dev/null; then
	printf "GRUB image cannot be created.\n"
	exit 1
else
	printf '\033[01;32m * \033[37mInstalling GRUB...\033[00;39m\n'
        grub-mkimage -d target/usr/lib/grub/x86_64-efi -c early.cfg -p boot -o efi64.exe -O x86_64-efi boot fat btrfs datetime disk ext2 gfxmenu help iso9660 ls luks lvm memdisk nilfs2 normal part_gpt part_msdos png scsi search linux reboot gfxterm gfxterm_background gfxterm_menu all_video
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
	mcopy efi64.exe A:/EFI/BOOT/bootx64.efi
	rm efi64.exe mtoolsrc
	mv efi64.img cdroot/efi.img
fi
