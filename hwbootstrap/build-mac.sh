#!/usr/bin/env bash

QTDIR=/Applications/Qt/6.2.2/macos

set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

echo $SCRIPT_DIR
rm -rf /tmp/hwbootstrap-mac-build
mkdir /tmp/hwbootstrap-mac-build
cd /tmp/hwbootstrap-mac-build
$QTDIR/bin/qmake QMAKE_APPLE_DEVICE_ARCHS=x86_64\ arm64 $SCRIPT_DIR
make
$QTDIR/bin/macdeployqt /tmp/hwbootstrap-mac-build/Hollywood.app
chmod +x /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Resources/*.sh
chmod +x /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Resources/*.js
xattr -d com.apple.quarantine /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Resources/*.sh
xattr -d com.apple.quarantine /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Resources/*.js
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Frameworks/QtOpenGL.framework
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Frameworks/QtQml.framework
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Frameworks/QtQmlModels.framework
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Frameworks/QtQuick.framework
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/Frameworks/QtVirtualKeyboard.framework
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/virtualkeyboard
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/platforminputcontexts
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/imageformats/libqwebp.dylib
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/imageformats/libqwbmp.dylib
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/imageformats/libtga.dylib
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/imageformats/libqtiff.dylib
rm -rf /tmp/hwbootstrap-mac-build/Hollywood.app/Contents/PlugIns/imageformats/libqico.dylib
rm -rf ~/hollywood-bootstrap-output/
mkdir ~/hollywood-bootstrap-output/
mv /tmp/hwbootstrap-mac-build/Hollywood.app ~/hollywood-bootstrap-output/
rm -rf /tmp//hwbootstrap-mac-build
