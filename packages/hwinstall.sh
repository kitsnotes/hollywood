#!/bin/sh
set -e

partition() {
	echo "*** Setting up disk /dev/$MYDEV for Hollywood Partitions"
	sgdisk -Z /dev/$MYDEV
	sgdisk -og /dev/$MYDEV
	sgdisk -n 1:2048:413695 -c 1:"EFI System Partition" -t 1:0700 /dev/$MYDEV
	sgdisk -n 2:413696:565247 -c 2:"Hollywood Boot" -t 2:8300 /dev/$MYDEV
	ENDSECTOR=`sgdisk -E /dev/$MYDEV`
	sgdisk -n 3:565248:$ENDSECTOR -c 3:"Hollywood System" -t 3:8300 /dev/$MYDEV
	sgdisk -p /dev/$MYDEV
}

if [ `id -u` -ne 0 ] ; then echo "Please run as root" ; exit 1 ; fi

echo "Hollywood Temporary Installer Script"
echo "(C) 2022 Originull Software.  This script is licensed under MIT License\n"
echo "*** Installing APK dependencies: e2fsprogs-extra dosfstools util-linux sgdisk efibootmgr"
apk add e2fsprogs-extra dosfstools util-linux sgdisk efibootmgr

echo "\n\n* * * * W A R N I N G ! * * * *"
echo "This script will ERASE ALL DATA on the disk you enter for installation. "
echo "Be ABSOLUTELY POSITIVE you are typing the CORRECT disk identifier below. "
echo "Hit Ctrl+C to exit this script and run fdisk NOW to check this if you are unsure.\n"
read -p "Enter the disk to install to (sda, vda, vdb, etc..) " MYDEV


#if [ ! -f "/dev/$MYDEV" ]; then
#    echo "The disk /dev/$MYDEV does not exist. Exiting."
#    exit
#fi

MACHINE_TYPE=`uname -m`
mirror="https://depot.originull.org"
chroot="chroot /mnt/"


echo "* * * W A R N I N G * * *"
echo "If you answer NO here your disks will STILL be formatted INCLUDING EFI system partition"
while true; do
    read -p "Do you wish to re-configure partitions for Hollywood defaults? (y/n) " yn
    case $yn in
        [Yy]* ) partition; break;;
        [Nn]* ) echo "OK Continuing On..."; break;;
        * ) echo "Please answer yes or no.";;
    esac
done


echo "*** Creating Filesystems"
mkfs.vfat "/dev/${MYDEV}1"
mkfs.ext4 -F "/dev/${MYDEV}2"
mkfs.ext4 -F "/dev/${MYDEV}3"

mount -t ext4 "/dev/${MYDEV}3" /mnt
mkdir -p /mnt/boot
mount -t ext4 "/dev/${MYDEV}2" /mnt/boot/
mkdir -p /mnt/boot/efi
mount -t vfat "/dev/${MYDEV}1" /mnt/boot/efi
mkdir -p /mnt/boot/efi/EFI/boot
mkdir -p /mnt/dev
mkdir -p /mnt/proc
mkdir -p /mnt/sys
mkdir -p /mnt/etc/apk/
mkdir -p /mnt/usr/lib
ln -s ./usr/lib /mnt/lib

if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	ln -s ./usr/lib /mnt/lib64
	ln -s ./lib /mnt/usr/lib64
fi

echo "$mirror/system" > /mnt/etc/apk/repositories
touch /tmp/.alpine-repo-override
apk --repositories-file /tmp/.alpine-repo-override \
	-X ${mirror}/system -U --allow-untrusted \
	-p /mnt --initdb add filesystem

apk --repositories-file /tmp/.alpine-repo-override \
        -X ${mirror}/system -U --allow-untrusted \
        -p /mnt add hollywood-base 

mount -o bind /dev /mnt/dev
mount -o bind /sys /mnt/sys
mount -o bind /proc /mnt/proc

cat << EOF > /mnt/etc/hosts
127.0.0.1	localhost

EOF

cat << EOF > /mnt/etc/resolv.conf
nameserver 9.9.9.9
nameserver 149.112.112.112
EOF

chroot /mnt /sbin/apk update
chroot /mnt /sbin/apk add linux-mainline
chroot /mnt /sbin/apk add refind
chroot /mnt /sbin/apk add hollywood
chroot /mnt /usr/sbin/rc-update add udev sysinit
chroot /mnt /usr/sbin/rc-update add devfs sysinit
chroot /mnt /usr/sbin/rc-update add dmesg sysinit
chroot /mnt /usr/sbin/rc-update add hwdrivers sysinit
chroot /mnt /usr/sbin/rc-update add acpid sysinit
chroot /mnt /usr/sbin/rc-update add agetty boot
chroot /mnt /usr/sbin/rc-update add hostname boot
chroot /mnt /usr/sbin/rc-update add modules boot
chroot /mnt /usr/sbin/rc-update add connman boot
chroot /mnt /usr/sbin/rc-update add dbus boot
chroot /mnt /usr/sbin/rc-update add hwclock boot
chroot /mnt /usr/sbin/rc-update add swap boot
chroot /mnt /usr/sbin/rc-update add udev-trigger boot
chroot /mnt /usr/sbin/rc-update add sysklogd boot
chroot /mnt /usr/sbin/rc-update add cronie boot
chroot /mnt /usr/sbin/rc-update add sysctl boot
chroot /mnt /usr/sbin/rc-update add bootmisc boot
chroot /mnt /usr/sbin/rc-update add urandom boot
chroot /mnt /usr/sbin/rc-update add openntpd boot

chroot /mnt /usr/sbin/rc-update add killprocs shutdown
chroot /mnt /usr/sbin/rc-update add savecache shutdown
chroot /mnt /usr/sbin/rc-update add mount-ro shutdown

chroot /mnt /usr/sbin/rc-update add agetty.tty1 default
chroot /mnt /usr/sbin/rc-update add sddm default

chroot /mnt /usr/sbin/addgroup hollywood
chroot /mnt /usr/sbin/adduser --disabled-password --gecos "Hollywood User" --shell /bin/zsh --ingroup hollywood hollywood
chroot /mnt /usr/sbin/adduser hollywood wheel
chroot /mnt /usr/sbin/adduser hollywood input
chroot /mnt /usr/sbin/adduser hollywood netdev
chroot /mnt /usr/sbin/adduser hollywood video
ln -s /usr/libexec/hollywood/session /mnt/usr/bin/hollwyood

ln -s /usr/share/applications/org.originull.terminull.desktop /mnt/home/hollywood/Desktop/Terminull.desktop
ln -s /usr/share/applications /mnt/home/hollywood/Desktop/Applications

mkdir -p /mnt/home/hollywood/.config/originull
cat << EOF > /mnt/home/hollywood/.config/originull/hollywood.conf
[Mouse]
DoubleClickSpeed=441

EOF

chroot /mnt /usr/bin/chown -R hollywood:hollywood /home/hollywood/

mkdir -p /mnt/boot/efi/EFI/hollywood/icons
mkdir -p /mnt/boot/efi/EFI/hollywood/tools
mkdir -p /mnt/boot/efi/EFI/hollywood/drivers
cp -r /mnt/usr/share/refind/icons/* /mnt/boot/efi/EFI/hollywood/icons
cp -r /mnt/usr/share/refind/drivers_x86_64/* /mnt/boot/efi/EFI/hollywood/drivers
cp -r /mnt/usr/share/refind/tools_x86_64/* /mnt/boot/efi/EFI/hollywood/tools
cp /mnt/usr/share/refind/refind_x64.efi /mnt/boot/efi/EFI/hollywood/bootx64.efi

cat << EOF > /mnt/etc/sddm.conf.d/10-hollywood.conf
[General]
DisplayServer=wayland
GreeterEnvironment=QT_QPA_PLATFORM=hollywood

[Wayland]
CompositorCommand=/usr/libexec/hollywood/sddm-compositor

EOF
cat << EOF > /mnt/etc/sddm.conf.d/autologin.conf
[Autologin]
User=hollywood
Session=hollywood

EOF

cat << EOF > /mnt/etc/ld.so.conf
# Begin /etc/ld.so.conf
/usr/local/lib
/opt/lib

EOF

cat << EOF > /mnt/etc/doas.d/doas.conf
permit persist :wheel

EOF

cat << EOF > /mnt/etc/nsswitch.conf
# Begin /etc/nsswitch.conf

passwd: files
group: files
shadow: files

hosts: files dns
networks: files

protocols: files
services: files
ethers: files
rpc: files

# End /etc/nsswitch.conf
EOF


PARTGUID=`sgdisk -i 2 /dev/${MYDEV} | grep "Partition unique" | cut -d":" -f2 | cut -d' ' -f2`
BLKID=`lsblk /dev/${MYDEV}3 -no UUID`
BLKIDBOOT=`lsblk /dev/${MYDEV}2 -no UUID`
BLKIDEFI=`lsblk /dev/${MYDEV}1 -no UUID`

cat << EOF > /mnt/boot/efi/EFI/hollywood/refind.conf
timeout 2
hideui banner
use_graphics_for linux, windows, osx
showtools shell, memtest, firmware
scanfor manual,external
scan_delay 0
dont_scan_volumes "Recovery HD"
fold_linux_kernels true

menuentry Hollywood {
	icon EFI/hollywood/icons/os_linux.png
	volume "Hollywood Boot"
	loader /vmlinuz-mainline
	initrd /initramfs-mainline
	options "root=UUID=${BLKID} ro quiet splash rootfstype=ext4"
}

EOF

cat << EOF > /mnt/etc/fstab
UUID=${BLKID}		  /           ext4    errors=remount-ro 1 1
UUID=${BLKIDBOOT}	  /boot       ext4    defaults 0 0
UUID=${BLKIDEFI}          /boot/efi   vfat    defauls 0 0
/dev/cdrom	/media/cdrom	iso9660	noauto,ro 0 0
/dev/usbdisk	/media/usb	vfat	noauto,ro 0 0

EOF

umount /mnt/dev
umount /mnt/sys
umount /mnt/proc
umount /mnt/boot/efi
umount /mnt/boot
umount /mnt

set +e
mount -t efivarfs efivarfs /sys/firmware/efi/efivars 
efibootmgr -c -d /dev/${MYDEV} -p 1 -L "Hollywood" -l "\efi\hollywood\bootx64.efi"

echo "*** Hollywood installation complete on /dev/${MYDEV} - please reboot"
