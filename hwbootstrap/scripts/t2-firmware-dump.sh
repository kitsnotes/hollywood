#!/usr/bin/env bash
#
# brcmfmac wireless & bluetooth firmware transfer utility
# This file originated from the T2Linux project and was modified 
# for use in the Hollywood operating system.
#
# Copyright (C) 2023 Originull Software <packages@originull.org>
# Copyright (C) 2022 Aditya Garg <gargaditya08@live.com>
# Copyright (C) 2022 Orlando Chamberlain <redecorating@protonmail.com>
#

set -euo pipefail
if [ "$(uname)" != "Darwin" ]; then
    echo "This script can only run on macOS. Exiting."
    exit 1
fi

if [ "$EUID" -ne 0 ]
    then echo "This script must be ran as the root superuser. Exiting."
    exit 1
fi

ver=$(sw_vers -productVersion | cut -d "." -f 1)
if [[ ${ver} < 12 ]]
then
    echo -e "This script is compatible only with macOS Monterey or later. Please upgrade your macOS."
    exit 1
fi
identifier=$(system_profiler SPHardwareDataType | grep "Model Identifier" | cut -d ":" -f 2 | xargs)
echo "Mounting the EFI partition"
diskutil mount disk0s1
mkdir -pv "/Volumes/EFI/hollywood/t2/" || return 0
echo "Getting Wi-Fi and Bluetooth firmware"
cd /usr/share/firmware
if [[ ${1-default} = -v ]]
then
    tar czvf /Volumes/EFI/hollywood/t2/apple-t2-firmware.tar.gz *
else
    tar czf /Volumes/EFI/hollywood/t2/apple-t2-firmware.tar.gz *
fi
if [[ (${identifier} = iMac19,1) || (${identifier} = iMac19,2) ]]
then
    nvramfile=$(ioreg -l | grep RequestedFiles | cut -d "/" -f 5 | rev | cut -c 4- | rev)
    txcapblob=$(ioreg -l | grep RequestedFiles | cut -d "/" -f 3 | cut -d "\"" -f 1)
    if [[ ${1-default} = -v ]]
    then
        cp -v /usr/share/firmware/wifi/C-4364__s-B2/${nvramfile} /Volumes/EFI/hollywood/t2/brcmfmac4364b2-pcie.txt
        cp -v /usr/share/firmware/wifi/C-4364__s-B2/${txcapblob} /Volumes/EFI/hollywood/t2/brcmfmac4364b2-pcie.txcap_blob
    else
        cp /usr/share/firmware/wifi/C-4364__s-B2/${nvramfile} /Volumes/EFI/hollywood/t2/brcmfmac4364b2-pcie.txt
        cp /usr/share/firmware/wifi/C-4364__s-B2/${txcapblob} /Volumes/EFI/hollywood/t2/brcmfmac4364b2-pcie.txcap_blob
    fi
fi

echo "Unmounting the EFI partition"
diskutil unmount disk0s1