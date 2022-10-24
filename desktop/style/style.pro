QT += core gui widgets

CONFIG += plugin
TARGET = hollywood
TEMPLATE = lib

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11
CONFIG += rtti_off exceptions_off

include(style.pri)

INCLUDEPATH += ./include

SOURCES += src/styleplugin.cc

HEADERS += include/styleplugin.h

OTHER_FILES += \
    styleplugin.json

g++ {
    QMAKE_CXXFLAGS *= -Wno-unused-function -Wno-unknown-pragmas
}
clang {
    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-format-security -Wno-unused-function -Wno-unknown-pragmas
}
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles
}
!isEmpty(target.path): INSTALLS += target
