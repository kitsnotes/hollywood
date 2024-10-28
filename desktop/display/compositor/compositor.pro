QT += waylandcompositor waylandcompositor-private gui-private
CONFIG += wayland-scanner
CONFIG -= wayland_compositor_quick

# This line is only relevant for Holywood distribution build
# It is otherwise harmless for an out-of-tree build
include(../../include/global.pri)

TARGET = hwcomp
INCLUDEPATH +=  include/core/ \
                include/protocol \
                include/xwayland \
                $$PWD/../driver/platformheaders

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

LIBS += -L../../output -lhweglfsplatformsupport
# LIBS for Xwayland support
LIBS += -lXcursor -lxcb-xfixes -lxcb-render -lxcb -lxcb-composite

contains(QT_CONFIG, no-pkg-config) {
    LIBS += -lwayland-server
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-server
}

HEADERS += \
    include/core/compositor.h \
    include/core/decoration.h \
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
    include/protocol/xdgshell_p.h \
    include/xwayland/xcbatom.h \
    include/xwayland/xcbatoms.h \
    include/xwayland/xcbcursors.h \
    include/xwayland/xcbproperties.h \
    include/xwayland/xcbresources.h \
    include/xwayland/xcbwindow.h \
    include/xwayland/xcbwrapper.h \
    include/xwayland/xwayland.h \
    include/xwayland/xwaylandserver.h \
    include/xwayland/xwaylandshellsurface.h

SOURCES += \
    src/core/decoration.cc \
    src/protocol/activation.cc \
    src/protocol/appmenu.cc \
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
    src/protocol/xdgshell.cc \
    src/xwayland/xcbatom.cc \
    src/xwayland/xcbatoms.cc \
    src/xwayland/xcbcursors.cc \
    src/xwayland/xcbproperties.cc \
    src/xwayland/xcbresources.cc \
    src/xwayland/xcbwindow.cc \
    src/xwayland/xcbwrapper.cc \
    src/xwayland/xwayland.cc \
    src/xwayland/xwaylandserver.cc \
    src/xwayland/xwaylandshellsurface.cc

#QMAKE_SUBSTITUTES += org.originull.compositor.desktop.in
#desktop.path = $$PREFIX/share/applications
#desktop.files = org.originull.compositor.desktop
target.path = $$PREFIX/bin


contains( DEFINES, BUILD_HOLLYWOOD )
{
    TARGET = compositor
    DESTDIR=$${OBJECTS_DIR}../../output/
    INCLUDEPATH+=../libshell/include/
    LIBS+=-lshell-$${HOLLYWOOD_APIVERSION}
    target.path = $$PREFIX/libexec/hollywood/
}

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
    shaders/surface.fsh \
    shaders/shadow.fsh \
    shaders/shadow.vsh \
    shaders/surface.vsh \
    shaders/transition.fsh \
    shaders/transition.vsh


RESOURCES += \
    resources.qrc

