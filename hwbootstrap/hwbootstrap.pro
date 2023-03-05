QT       += core gui svg widgets
QT -= quick qml dbus virtualkeyboard
CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
TEMPLATE = app
include("../desktop/style/style.pri")

INCLUDEPATH += ../desktop/style/include

SOURCES += \
    bootstrap.cc \
    menu.cc

HEADERS += \
    bootstrap.h \
    menu.h

RESOURCES += \
    res.qrc


macx {
    TARGET = Hollywood
    CONFIG += sdk_nno_version_check
    QMAKE_INFO_PLIST = res/Info.plist
    ICON = res/hollywood.icns
    RESOURCE_FILES.files = $$ICON \
        scripts/askpass-macos.js \
        scripts/t2-firmware-dump.sh
    RESOURCE_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += RESOURCE_FILES
    LIBS += -framework CoreFoundation
}

win32 {
    TARGET = Hollywood
    RC_FILE = res/hwbootstrap.rc
    CONFIG -= embed_manifest_exe
}

DISTFILES += \
    res/firmware-list.txt
