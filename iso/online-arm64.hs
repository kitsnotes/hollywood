version edge
network false
hostname hollywood-live
# "live"
rootpw $6$b3P7y2vLSRl3ujqi$mYk5jlsRta8RQ/Qvunadj5ZO8BYO7cLkJsrMKve/r5Ka/1Gqrh1v93cjEttRR3wdmKwyvZZnkLbaw4T1dIEL40
nameserver 2620:fe::fe
nameserver 9.9.9.9
nameserver 149.112.112.112

# Base packages
pkginstall hollywood-desktop hollywood-setup sddm livecd-support dmidecode efibootmgr
# temporary ISO debugging things
pkginstall gdb cmst

kernel stable
pkginstall linux-asahi

# Live User
username hollywood
useralias hollywood Hollywood Live Image User
usergroups hollywood users,audio,video,wheel,netdev,input
#userpw hollywood $6$b3P7y2vLSRl3ujqi$mYk5jlsRta8RQ/Qvunadj5ZO8BYO7cLkJsrMKve/r5Ka/1Gqrh1v93cjEttRR3wdmKwyvZZnkLbaw4T1dIEL40

svcenable udev sysinit
svcenable udev-trigger sysinit

svcenable sysklogd sysinit
svcenable urandom sysinit
svcenable loadkeys sysinit
svcenable bootmisc sysinit
svcenable sysctl sysinit
svcenable binfmt boot
svcenable cronie boot
svcenable rtkit boot
svcenable dbus boot
svcenable openntpd boot
svcenable connman boot
svcenable elogind boot
svcenable sddm default
svcenable wpa_supplicant boot
svcenable agetty.tty1 default
svcenable polkit boot

svcenable killprocs shutdown
svcenable savecache shutdown
svcenable mount-ro shutdown

arch aarch64
signingkey /etc/apk/keys/pkg-arm64@originull.org-626fa45d.rsa.pub