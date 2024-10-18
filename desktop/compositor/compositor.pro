include(../include/global.pri)

CONFIG += wayland-scanner
QT += waylandcompositor waylandcompositor-private gui-private
CONFIG -= wayland_compositor_quick

TARGET = compositor
INCLUDEPATH += include/
INCLUDEPATH += ../libshell/include/
INCLUDEPATH += $$PWD/../eglfs/platformheaders/
WAYLANDSERVERSOURCES += protocols/originull-privateapi.xml
WAYLANDSERVERSOURCES += protocols/appmenu.xml
WAYLANDSERVERSOURCES += protocols/plasma-window-management.xml
WAYLANDSERVERSOURCES += protocols/fullscreen-shell-unstable-v1.xml
WAYLANDSERVERSOURCES += protocols/wlr-layer-shell-unstable-v1.xml
WAYLANDSERVERSOURCES += protocols/wlr-screencopy-unstable-v1.xml
WAYLANDSERVERSOURCES += protocols/gtk.xml
WAYLANDSERVERSOURCES += protocols/qt-shell-unstable-v1.xml
WAYLANDSERVERSOURCES += protocols/xdg-shell.xml
WAYLANDSERVERSOURCES += protocols/xdg-activation-v1.xml
WAYLANDSERVERSOURCES += protocols/pointer-constraints-unstable-v1.xml
WAYLANDSERVERSOURCES += protocols/relative-pointer-unstable-v1.xml


HEADERS += \
    include/activation.h \
    include/application.h \
    include/appmenu.h \
    include/blitter.h \
    include/fullscreen.h \
    include/gtkshell.h \
    include/layershell.h \
    include/originull.h \
    include/output.h \
    include/pointerconstraints.h \
    include/qtshell.h \
    include/relativepointer.h \
    include/screencopy.h \
    include/shortcuts.h \
    include/surfaceobject.h \
    include/outputwnd.h \
    include/view.h \
    include/compositor.h \
    include/wndmgmt.h \
    include/wallpaper.h \
    include/xdgdialog.h \
    include/xdgshell.h \
    include/xdgshell_p.h

SOURCES += \
    src/activation.cc \
    src/application.cc \
    src/appmenu.cc \
    src/blitter.cc \
    src/fullscreen.cc \
    src/gtkshell.cc \
    src/layershell.cc \
    src/originull.cc \
    src/output.cc \
    src/pointerconstraints.cc \
    src/qtshell.cc \
    src/relativepointer.cc \
    src/screencopy.cc \
    src/shortcuts.cc \
    src/surfaceobject.cc \
    src/compositor.cc \
    src/outputwnd.cc \
    src/view.cc \
    src/wndmgmt.cc \
    src/wallpaper.cc \
    src/xdgdialog.cc \
    src/xdgshell.cc

#QMAKE_SUBSTITUTES += org.originull.compositor.desktop.in
#desktop.path = $$PREFIX/share/applications
#desktop.files = org.originull.compositor.desktop
target.path = $$PREFIX/libexec/hollywood/

INSTALLS += target
# desktop

DISTFILES += \
    org.originull.compositor.desktop.in \
    protocols/appmenu.xml \
    protocols/gtk.xml \
    protocols/originull-privateapi.xml \
    protocols/plasma-window-management.xml \
    protocols/wlr-layer-shell-unstable-v1.xml \
    protocols/qt-shell-unstable-v1.xml \
    protocols/xdg-shell.xml \
    rgba.fsh \
    rgbconv.fsh \
    rgbconv.vsh \
    shadow.fsh \
    shadow.vsh \
    surface.vsh \
    transition.fsh \
    transition.vsh

LIBS += -lHollywoodEglfsPlatformSupport -L../eglfs/platformheaders  -L../output -lshell-$${HOLLYWOOD_APIVERSION}

contains(QT_CONFIG, no-pkg-config) {
    LIBS += -lwayland-server
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-server
}

RESOURCES += \
    resources.qrc
