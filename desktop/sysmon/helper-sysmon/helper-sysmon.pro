include(../../include/global.pri)

QT -= gui
TARGET=sysmon
CONFIG += c++17 console
CONFIG -= app_bundle
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
        main.cc

target.path = $$PREFIX/libexec/hollywood/helpers/
INSTALLS += target
