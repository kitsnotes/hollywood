version edge
network false
hostname hollywood-live
# "live"
rootpw $6$b3P7y2vLSRl3ujqi$mYk5jlsRta8RQ/Qvunadj5ZO8BYO7cLkJsrMKve/r5Ka/1Gqrh1v93cjEttRR3wdmKwyvZZnkLbaw4T1dIEL40
nameserver 2620:fe::fe
nameserver 9.9.9.9
nameserver 149.112.112.112

kernel stable
# Base packages
pkginstall hollywood-desktop livecd-support hollywood-setup sddm dmidecode efibootmgr apple-bce-stable
# temporary ISO debugging things
pkginstall gdb cmst

# Live User
username hollywood
useralias hollywood Hollywood Live Image User
usergroups hollywood users,audio,video,wheel,netdev,input
userpw hollywood $6$b3P7y2vLSRl3ujqi$mYk5jlsRta8RQ/Qvunadj5ZO8BYO7cLkJsrMKve/r5Ka/1Gqrh1v93cjEttRR3wdmKwyvZZnkLbaw4T1dIEL40

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

arch x86_64
signingkey /etc/apk/keys/pkg-amd64@originull.org-62bcc3cd.rsa.pub