include(../include/global.pri)

QT       += charts

INCLUDEPATH += include/
INCLUDEPATH += ../libcommdlg/include/

TARGET=hwcalc
LIBS += -L../libcommdlg -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION}

HEADERS       = \
                calculator.h \
                window.h
SOURCES       = \
                calculator.cc \
                window.cc

DISTFILES += \
    org.originull.calculator.desktop.in

QMAKE_SUBSTITUTES += org.originull.calculator.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.calculator.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop
