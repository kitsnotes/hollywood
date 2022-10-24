include(../../include/global.pri)

QT -= gui widgets
TARGET=datetime

INCLUDEPATH += ../../include
CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
        main.cc

target.path = $$PREFIX/libexec/hollywood/helpers/
INSTALLS += target
