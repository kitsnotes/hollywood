include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=02-wallpaper

SOURCES += \
    chooser.cc \
    delegate.cc \
    wpapplet.cc \
    wpmodel.cc

HEADERS += \
    chooser.h \
    delegate.h \
    wpapplet.h \
    wpmodel.h

DISTFILES += applet.json
target.path = $$PREFIX/libexec/hollywood/settings

!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    wp.qrc

