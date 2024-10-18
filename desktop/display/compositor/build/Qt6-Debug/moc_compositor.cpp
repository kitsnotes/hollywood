/****************************************************************************
** Meta object code from reading C++ file 'compositor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/compositor.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'compositor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCompositorENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCompositorENDCLASS = QtMocHelpers::stringData(
    "Compositor",
    "startMove",
    "",
    "startResize",
    "edge",
    "anchored",
    "dragStarted",
    "Surface*",
    "dragIcon",
    "frameOffset",
    "offset",
    "settingsChanged",
    "triggerRender",
    "lockSession",
    "surfaceHasContentChanged",
    "onStartMove",
    "QWaylandSeat*",
    "seat",
    "onWlStartResize",
    "QWaylandWlShellSurface::ResizeEdge",
    "edges",
    "onXdgStartResize",
    "Qt::Edges",
    "startDrag",
    "onSurfaceCreated",
    "QWaylandSurface*",
    "surface",
    "onMenuServerRequest",
    "OriginullMenuServer*",
    "menu",
    "onDesktopRequest",
    "onWlShellSurfaceCreated",
    "QWaylandWlShellSurface*",
    "wlShellSurface",
    "onXdgSurfaceCreated",
    "HWWaylandXdgSurface*",
    "xdgSurface",
    "onXdgTopLevelCreated",
    "HWWaylandXdgToplevel*",
    "toplevel",
    "onXdgPopupCreated",
    "HWWaylandXdgPopup*",
    "popup",
    "onLayerSurfaceCreated",
    "WlrLayerSurfaceV1*",
    "layerSurface",
    "onGtkSurfaceCreated",
    "GtkSurface*",
    "gtkSurface",
    "onQtSurfaceCreated",
    "QtSurface*",
    "qtSurface",
    "onFullscreenSurfaceRequested",
    "onSetTransient",
    "parentSurface",
    "relativeToParent",
    "inactive",
    "onSetPopup",
    "parent",
    "onSubsurfaceChanged",
    "child",
    "onSubsurfacePositionChanged",
    "position",
    "onXdgSurfaceActivated",
    "updateCursor",
    "onRotateWallpaper",
    "appMenuCreated",
    "AppMenu*",
    "m",
    "menuServerDestroyed",
    "configChanged",
    "loadSettings",
    "setupIdleTimer",
    "idleTimeout"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCompositorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  230,    2, 0x06,    1 /* Public */,
       3,    2,  231,    2, 0x06,    2 /* Public */,
       6,    1,  236,    2, 0x06,    5 /* Public */,
       9,    1,  239,    2, 0x06,    7 /* Public */,
      11,    0,  242,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,  243,    2, 0x0a,   10 /* Public */,
      13,    0,  244,    2, 0x0a,   11 /* Public */,
      14,    0,  245,    2, 0x09,   12 /* Protected */,
      15,    1,  246,    2, 0x09,   13 /* Protected */,
      18,    2,  249,    2, 0x09,   15 /* Protected */,
      21,    2,  254,    2, 0x09,   18 /* Protected */,
      23,    0,  259,    2, 0x09,   21 /* Protected */,
      24,    1,  260,    2, 0x09,   22 /* Protected */,
      27,    1,  263,    2, 0x09,   24 /* Protected */,
      30,    1,  266,    2, 0x09,   26 /* Protected */,
      31,    1,  269,    2, 0x09,   28 /* Protected */,
      34,    1,  272,    2, 0x09,   30 /* Protected */,
      37,    2,  275,    2, 0x09,   32 /* Protected */,
      40,    2,  280,    2, 0x09,   35 /* Protected */,
      43,    1,  285,    2, 0x09,   38 /* Protected */,
      46,    1,  288,    2, 0x09,   40 /* Protected */,
      49,    1,  291,    2, 0x09,   42 /* Protected */,
      52,    1,  294,    2, 0x09,   44 /* Protected */,
      53,    3,  297,    2, 0x09,   46 /* Protected */,
      57,    3,  304,    2, 0x09,   50 /* Protected */,
      59,    2,  311,    2, 0x09,   54 /* Protected */,
      61,    1,  316,    2, 0x09,   57 /* Protected */,
      63,    1,  319,    2, 0x09,   59 /* Protected */,
      64,    0,  322,    2, 0x09,   61 /* Protected */,
      65,    0,  323,    2, 0x09,   62 /* Protected */,
      66,    1,  324,    2, 0x08,   63 /* Private */,
      69,    0,  327,    2, 0x08,   65 /* Private */,
      70,    0,  328,    2, 0x08,   66 /* Private */,
      71,    0,  329,    2, 0x08,   67 /* Private */,
      72,    0,  330,    2, 0x08,   68 /* Private */,
      73,    0,  331,    2, 0x08,   69 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QPoint,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 19,   17,   20,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 22,   17,   20,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25,   26,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 25,   26,
    QMetaType::Void, 0x80000000 | 32,   33,
    QMetaType::Void, 0x80000000 | 35,   36,
    QMetaType::Void, 0x80000000 | 38, 0x80000000 | 35,   39,   26,
    QMetaType::Void, 0x80000000 | 41, 0x80000000 | 35,   42,   26,
    QMetaType::Void, 0x80000000 | 44,   45,
    QMetaType::Void, 0x80000000 | 47,   48,
    QMetaType::Void, 0x80000000 | 50,   51,
    QMetaType::Void, 0x80000000 | 25,   26,
    QMetaType::Void, 0x80000000 | 25, QMetaType::QPoint, QMetaType::Bool,   54,   55,   56,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 25, QMetaType::QPoint,   17,   58,   55,
    QMetaType::Void, 0x80000000 | 25, 0x80000000 | 25,   60,   58,
    QMetaType::Void, QMetaType::QPoint,   62,
    QMetaType::Void, 0x80000000 | 25,   26,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 67,   68,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Compositor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandCompositor::staticMetaObject>(),
    qt_meta_stringdata_CLASSCompositorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCompositorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCompositorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Compositor, std::true_type>,
        // method 'startMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startResize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'dragStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Surface *, std::false_type>,
        // method 'frameOffset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'settingsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'triggerRender'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'lockSession'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'surfaceHasContentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        // method 'onWlStartResize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandWlShellSurface::ResizeEdge, std::false_type>,
        // method 'onXdgStartResize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::Edges, std::false_type>,
        // method 'startDrag'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        // method 'onMenuServerRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<OriginullMenuServer *, std::false_type>,
        // method 'onDesktopRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        // method 'onWlShellSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandWlShellSurface *, std::false_type>,
        // method 'onXdgSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface *, std::false_type>,
        // method 'onXdgTopLevelCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel *, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface *, std::false_type>,
        // method 'onXdgPopupCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgPopup *, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface *, std::false_type>,
        // method 'onLayerSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<WlrLayerSurfaceV1 *, std::false_type>,
        // method 'onGtkSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<GtkSurface *, std::false_type>,
        // method 'onQtSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtSurface *, std::false_type>,
        // method 'onFullscreenSurfaceRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        // method 'onSetTransient'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onSetPopup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'onSubsurfaceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        // method 'onSubsurfacePositionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'onXdgSurfaceActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        // method 'updateCursor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRotateWallpaper'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'appMenuCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<AppMenu *, std::false_type>,
        // method 'menuServerDestroyed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'configChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setupIdleTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'idleTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Compositor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Compositor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->startMove(); break;
        case 1: _t->startResize((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->dragStarted((*reinterpret_cast< std::add_pointer_t<Surface*>>(_a[1]))); break;
        case 3: _t->frameOffset((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 4: _t->settingsChanged(); break;
        case 5: _t->triggerRender(); break;
        case 6: _t->lockSession(); break;
        case 7: _t->surfaceHasContentChanged(); break;
        case 8: _t->onStartMove((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1]))); break;
        case 9: _t->onWlStartResize((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWaylandWlShellSurface::ResizeEdge>>(_a[2]))); break;
        case 10: _t->onXdgStartResize((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Qt::Edges>>(_a[2]))); break;
        case 11: _t->startDrag(); break;
        case 12: _t->onSurfaceCreated((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1]))); break;
        case 13: _t->onMenuServerRequest((*reinterpret_cast< std::add_pointer_t<OriginullMenuServer*>>(_a[1]))); break;
        case 14: _t->onDesktopRequest((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1]))); break;
        case 15: _t->onWlShellSurfaceCreated((*reinterpret_cast< std::add_pointer_t<QWaylandWlShellSurface*>>(_a[1]))); break;
        case 16: _t->onXdgSurfaceCreated((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgSurface*>>(_a[1]))); break;
        case 17: _t->onXdgTopLevelCreated((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgToplevel*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<HWWaylandXdgSurface*>>(_a[2]))); break;
        case 18: _t->onXdgPopupCreated((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgPopup*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<HWWaylandXdgSurface*>>(_a[2]))); break;
        case 19: _t->onLayerSurfaceCreated((*reinterpret_cast< std::add_pointer_t<WlrLayerSurfaceV1*>>(_a[1]))); break;
        case 20: _t->onGtkSurfaceCreated((*reinterpret_cast< std::add_pointer_t<GtkSurface*>>(_a[1]))); break;
        case 21: _t->onQtSurfaceCreated((*reinterpret_cast< std::add_pointer_t<QtSurface*>>(_a[1]))); break;
        case 22: _t->onFullscreenSurfaceRequested((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1]))); break;
        case 23: _t->onSetTransient((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 24: _t->onSetPopup((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[3]))); break;
        case 25: _t->onSubsurfaceChanged((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[2]))); break;
        case 26: _t->onSubsurfacePositionChanged((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 27: _t->onXdgSurfaceActivated((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1]))); break;
        case 28: _t->updateCursor(); break;
        case 29: _t->onRotateWallpaper(); break;
        case 30: _t->appMenuCreated((*reinterpret_cast< std::add_pointer_t<AppMenu*>>(_a[1]))); break;
        case 31: _t->menuServerDestroyed(); break;
        case 32: _t->configChanged(); break;
        case 33: _t->loadSettings(); break;
        case 34: _t->setupIdleTimer(); break;
        case 35: _t->idleTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSeat* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSeat* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSeat* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandWlShellSurface* >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgSurface* >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgSurface* >(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgToplevel* >(); break;
            }
            break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgPopup* >(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgSurface* >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        case 23:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSeat* >(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        case 25:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        case 27:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Compositor::*)();
            if (_t _q_method = &Compositor::startMove; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Compositor::*)(int , bool );
            if (_t _q_method = &Compositor::startResize; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Compositor::*)(Surface * );
            if (_t _q_method = &Compositor::dragStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Compositor::*)(const QPoint & );
            if (_t _q_method = &Compositor::frameOffset; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Compositor::*)();
            if (_t _q_method = &Compositor::settingsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *Compositor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Compositor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCompositorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWaylandCompositor::qt_metacast(_clname);
}

int Compositor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandCompositor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    return _id;
}

// SIGNAL 0
void Compositor::startMove()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Compositor::startResize(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Compositor::dragStarted(Surface * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Compositor::frameOffset(const QPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Compositor::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
