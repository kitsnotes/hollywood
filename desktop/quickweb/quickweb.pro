include(../include/global.pri)

TEMPLATE = app
TARGET = quickweb
QT += webenginewidgets

HEADERS += \
    browser.h \
    browserwindow.h \
    downloadmanagerwidget.h \
    downloadwidget.h \
    tabwidget.h \
    webpage.h \
    webpopupwindow.h \
    webview.h

SOURCES += \
    browser.cpp \
    browserwindow.cpp \
    downloadmanagerwidget.cpp \
    downloadwidget.cpp \
    main.cpp \
    tabwidget.cpp \
    webpage.cpp \
    webpopupwindow.cpp \
    webview.cpp

FORMS += \
    certificateerrordialog.ui \
    passworddialog.ui \
    downloadmanagerwidget.ui \
    downloadwidget.ui

RESOURCES += data/simplebrowser.qrc

# install
QMAKE_SUBSTITUTES +=org.originull.quickweb.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.quickweb.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop
