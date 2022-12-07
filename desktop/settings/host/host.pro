include(../../include/global.pri)

TARGET = hwsettings

INCLUDEPATH += ../../libcommdlg
SOURCES += \
qcategorizedview/qcategorizedsortfilterproxymodel.cpp \
qcategorizedview/qcategorizedview.cpp \
qcategorizedview/qcategorydrawer.cpp \
    settings.cc \
settingsmodel.cc

HEADERS += \
qcategorizedview/qcategorizedsortfilterproxymodel.h \
qcategorizedview/qcategorizedsortfilterproxymodel_p.h \
qcategorizedview/qcategorizedview.h \
qcategorizedview/qcategorizedview_p.h \
qcategorizedview/qcategorydrawer.h \
    settings.h \
settingsmodel.h
LIBS += -L../../output -lcommdlg-$${HOLLYWOOD_APIVERSION}
# Default rules for deployment.
QMAKE_SUBSTITUTES +=org.originull.hwsettings.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.hwsettings.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop

DISTFILES += \
org.originull.hwsettings.desktop.in
