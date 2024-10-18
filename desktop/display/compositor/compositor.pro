include(../../include/global.pri)
DESTDIR=$${OBJECTS_DIR}../../output/

CONFIG += wayland-scanner
QT += waylandcompositor waylandcompositor-private gui-private
CONFIG -= wayland_compositor_quick

TARGET = compositor
INCLUDEPATH +=  include/core/ \
                include/protocol \
                ../libshell/include/

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
    include/core/blitter.h \
    include/core/compositor.h \
    include/core/output.h \
    include/core/shortcuts.h \
    include/core/surfaceobject.h \
    include/core/outputwnd.h \
    include/core/wallpaper.h \
    include/core/view.h \
    include/protocol/activation.h \
    include/protocol/appmenu.h \
    include/protocol/fullscreen.h \
    include/protocol/gtkshell.h \
    include/protocol/layershell.h \
    include/protocol/originull.h \
    include/protocol/qtshell.h \
    include/protocol/relativepointer.h \
    include/protocol/pointerconstraints.h \
    include/protocol/screencopy.h \
    include/protocol/wndmgmt.h \
    include/protocol/xdgdialog.h \
    include/protocol/xdgshell.h \
    include/protocol/xdgshell_p.h

SOURCES += \
    src/protocol/activation.cc \
    src/protocol/appmenu.cc \
    src/core/blitter.cc \
    src/protocol/fullscreen.cc \
    src/protocol/gtkshell.cc \
    src/protocol/layershell.cc \
    src/protocol/originull.cc \
    src/core/output.cc \
    src/protocol/pointerconstraints.cc \
    src/protocol/qtshell.cc \
    src/protocol/relativepointer.cc \
    src/protocol/screencopy.cc \
    src/core/shortcuts.cc \
    src/core/surfaceobject.cc \
    src/core/compositor.cc \
    src/core/outputwnd.cc \
    src/core/view.cc \
    src/protocol/wndmgmt.cc \
    src/core/wallpaper.cc \
    src/protocol/xdgdialog.cc \
    src/protocol/xdgshell.cc

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

LIBS += -L../../output -lhweglfsplatformsupport \
     -lshell-$${HOLLYWOOD_APIVERSION}

contains(QT_CONFIG, no-pkg-config) {
    LIBS += -lwayland-server
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-server
}

RESOURCES += \
    resources.qrc
