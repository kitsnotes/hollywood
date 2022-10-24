TARGET = QtEdidSupport
TEMPLATE = lib
QT = core-private
CONFIG += static

INCLUDEPATH += $$PWD/hollywood

DEFINES += QT_NO_CAST_FROM_ASCII

HEADERS += hollywood/private/qedidparser_p.h \
            hollywood/private/qedidvendortable_p.h
SOURCES += qedidparser.cpp

