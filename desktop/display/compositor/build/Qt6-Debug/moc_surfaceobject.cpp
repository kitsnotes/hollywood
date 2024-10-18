/****************************************************************************
** Meta object code from reading C++ file 'surfaceobject.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/surfaceobject.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'surfaceobject.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSSurfaceENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSurfaceENDCLASS = QtMocHelpers::stringData(
    "Surface",
    "primaryViewChanged",
    "",
    "geometryChanged",
    "activate",
    "deactivate",
    "toggleMinimize",
    "toggleMaximize",
    "toggleActive",
    "sendClose",
    "recalculateLayerShellAnchorPosition",
    "onLayerShellSizeChanged",
    "invalidateCachedDecoration",
    "onChildAdded",
    "QWaylandSurface*",
    "child",
    "onSurfaceSourceGeometryChanged",
    "onDestinationSizeChanged",
    "onBufferScaleChanged",
    "onXdgStartResize",
    "QWaylandSeat*",
    "seat",
    "Qt::Edges",
    "edges",
    "onXdgSetMaximized",
    "onXdgUnsetMaximized",
    "onXdgSetMinimized",
    "unsetMinimized",
    "onXdgSetFullscreen",
    "QWaylandOutput*",
    "output",
    "onXdgUnsetFullscreen",
    "onXdgTitleChanged",
    "onXdgParentTopLevelChanged",
    "onXdgAppIdChanged",
    "onXdgWindowGeometryChanged",
    "completeXdgConfigure",
    "surfaceDestroyed",
    "viewSurfaceDestroyed",
    "reconfigureLayerSurface",
    "onLayerChanged",
    "onLayerShellXdgPopupParentChanged",
    "HWWaylandXdgPopup*",
    "popup",
    "onAnchorsChanged",
    "onExclusiveZoneChanged",
    "decorationModeChanged",
    "onQtWindowTitleChanged",
    "onQtShellActivationRequest",
    "onQtShellReposition",
    "pos",
    "onQtShellSetSize",
    "size",
    "onQtWindowFlagsChanged",
    "Qt::WindowFlags",
    "f",
    "surfacePosition",
    "animatedSurfaceSize",
    "SurfaceType",
    "Unknown",
    "TopLevel",
    "Transient",
    "Popup",
    "TopLevelTool",
    "MenuServer",
    "LayerShell",
    "Desktop"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSurfaceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   14, // methods
       2,  312, // properties
       1,  322, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  254,    2, 0x06,    4 /* Public */,
       3,    0,  255,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,  256,    2, 0x0a,    6 /* Public */,
       5,    0,  257,    2, 0x0a,    7 /* Public */,
       6,    0,  258,    2, 0x0a,    8 /* Public */,
       7,    0,  259,    2, 0x0a,    9 /* Public */,
       8,    0,  260,    2, 0x0a,   10 /* Public */,
       9,    0,  261,    2, 0x0a,   11 /* Public */,
      10,    0,  262,    2, 0x0a,   12 /* Public */,
      11,    0,  263,    2, 0x0a,   13 /* Public */,
      12,    0,  264,    2, 0x0a,   14 /* Public */,
      13,    1,  265,    2, 0x08,   15 /* Private */,
      16,    0,  268,    2, 0x08,   17 /* Private */,
      17,    0,  269,    2, 0x08,   18 /* Private */,
      18,    0,  270,    2, 0x08,   19 /* Private */,
      19,    2,  271,    2, 0x08,   20 /* Private */,
      24,    0,  276,    2, 0x08,   23 /* Private */,
      25,    0,  277,    2, 0x08,   24 /* Private */,
      26,    0,  278,    2, 0x08,   25 /* Private */,
      27,    0,  279,    2, 0x08,   26 /* Private */,
      28,    1,  280,    2, 0x08,   27 /* Private */,
      31,    0,  283,    2, 0x08,   29 /* Private */,
      32,    0,  284,    2, 0x08,   30 /* Private */,
      33,    0,  285,    2, 0x08,   31 /* Private */,
      34,    0,  286,    2, 0x08,   32 /* Private */,
      35,    0,  287,    2, 0x08,   33 /* Private */,
      36,    0,  288,    2, 0x08,   34 /* Private */,
      37,    0,  289,    2, 0x08,   35 /* Private */,
      38,    0,  290,    2, 0x08,   36 /* Private */,
      39,    0,  291,    2, 0x08,   37 /* Private */,
      40,    0,  292,    2, 0x08,   38 /* Private */,
      41,    1,  293,    2, 0x08,   39 /* Private */,
      44,    0,  296,    2, 0x08,   41 /* Private */,
      45,    0,  297,    2, 0x08,   42 /* Private */,
      46,    0,  298,    2, 0x08,   43 /* Private */,
      47,    1,  299,    2, 0x08,   44 /* Private */,
      48,    0,  302,    2, 0x08,   46 /* Private */,
      49,    1,  303,    2, 0x08,   47 /* Private */,
      51,    1,  306,    2, 0x08,   49 /* Private */,
      53,    1,  309,    2, 0x08,   51 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 22,   21,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 29,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 42,   43,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   50,
    QMetaType::Void, QMetaType::QSize,   52,
    QMetaType::Void, 0x80000000 | 54,   55,

 // properties: name, type, flags, notifyId, revision
      56, QMetaType::QPointF, 0x00015003, uint(-1), 0,
      57, QMetaType::QSize, 0x00015103, uint(-1), 0,

 // enums: name, alias, flags, count, data
      58,   58, 0x0,    8,  327,

 // enum data: key, value
      59, uint(Surface::Unknown),
      60, uint(Surface::TopLevel),
      61, uint(Surface::Transient),
      62, uint(Surface::Popup),
      63, uint(Surface::TopLevelTool),
      64, uint(Surface::MenuServer),
      65, uint(Surface::LayerShell),
      66, uint(Surface::Desktop),

       0        // eod
};

Q_CONSTINIT const QMetaObject Surface::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSSurfaceENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSurfaceENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSurfaceENDCLASS_t,
        // property 'surfacePosition'
        QtPrivate::TypeAndForceComplete<QPointF, std::true_type>,
        // property 'animatedSurfaceSize'
        QtPrivate::TypeAndForceComplete<QSize, std::true_type>,
        // enum 'SurfaceType'
        QtPrivate::TypeAndForceComplete<Surface::SurfaceType, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Surface, std::true_type>,
        // method 'primaryViewChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'geometryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'deactivate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleMinimize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleMaximize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleActive'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'recalculateLayerShellAnchorPosition'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLayerShellSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'invalidateCachedDecoration'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onChildAdded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        // method 'onSurfaceSourceGeometryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDestinationSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onBufferScaleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgStartResize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::Edges, std::false_type>,
        // method 'onXdgSetMaximized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgUnsetMaximized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgSetMinimized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unsetMinimized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgSetFullscreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandOutput *, std::false_type>,
        // method 'onXdgUnsetFullscreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgTitleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgParentTopLevelChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgAppIdChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onXdgWindowGeometryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'completeXdgConfigure'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'surfaceDestroyed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'viewSurfaceDestroyed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reconfigureLayerSurface'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLayerChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLayerShellXdgPopupParentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgPopup *, std::false_type>,
        // method 'onAnchorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExclusiveZoneChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'decorationModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onQtWindowTitleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onQtShellActivationRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onQtShellReposition'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'onQtShellSetSize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        // method 'onQtWindowFlagsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Qt::WindowFlags &, std::false_type>
    >,
    nullptr
} };

void Surface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Surface *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->primaryViewChanged(); break;
        case 1: _t->geometryChanged(); break;
        case 2: _t->activate(); break;
        case 3: _t->deactivate(); break;
        case 4: _t->toggleMinimize(); break;
        case 5: _t->toggleMaximize(); break;
        case 6: _t->toggleActive(); break;
        case 7: _t->sendClose(); break;
        case 8: _t->recalculateLayerShellAnchorPosition(); break;
        case 9: _t->onLayerShellSizeChanged(); break;
        case 10: _t->invalidateCachedDecoration(); break;
        case 11: _t->onChildAdded((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1]))); break;
        case 12: _t->onSurfaceSourceGeometryChanged(); break;
        case 13: _t->onDestinationSizeChanged(); break;
        case 14: _t->onBufferScaleChanged(); break;
        case 15: _t->onXdgStartResize((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Qt::Edges>>(_a[2]))); break;
        case 16: _t->onXdgSetMaximized(); break;
        case 17: _t->onXdgUnsetMaximized(); break;
        case 18: _t->onXdgSetMinimized(); break;
        case 19: _t->unsetMinimized(); break;
        case 20: _t->onXdgSetFullscreen((*reinterpret_cast< std::add_pointer_t<QWaylandOutput*>>(_a[1]))); break;
        case 21: _t->onXdgUnsetFullscreen(); break;
        case 22: _t->onXdgTitleChanged(); break;
        case 23: _t->onXdgParentTopLevelChanged(); break;
        case 24: _t->onXdgAppIdChanged(); break;
        case 25: _t->onXdgWindowGeometryChanged(); break;
        case 26: _t->completeXdgConfigure(); break;
        case 27: _t->surfaceDestroyed(); break;
        case 28: _t->viewSurfaceDestroyed(); break;
        case 29: _t->reconfigureLayerSurface(); break;
        case 30: _t->onLayerChanged(); break;
        case 31: _t->onLayerShellXdgPopupParentChanged((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgPopup*>>(_a[1]))); break;
        case 32: _t->onAnchorsChanged(); break;
        case 33: _t->onExclusiveZoneChanged(); break;
        case 34: _t->decorationModeChanged(); break;
        case 35: _t->onQtWindowTitleChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->onQtShellActivationRequest(); break;
        case 37: _t->onQtShellReposition((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 38: _t->onQtShellSetSize((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1]))); break;
        case 39: _t->onQtWindowFlagsChanged((*reinterpret_cast< std::add_pointer_t<Qt::WindowFlags>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 11:
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
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSeat* >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandOutput* >(); break;
            }
            break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgPopup* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Surface::*)();
            if (_t _q_method = &Surface::primaryViewChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Surface::*)();
            if (_t _q_method = &Surface::geometryChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Surface *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QPointF*>(_v) = _t->surfacePosition(); break;
        case 1: *reinterpret_cast< QSize*>(_v) = _t->surfaceSize(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Surface *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setPosition(*reinterpret_cast< QPointF*>(_v)); break;
        case 1: _t->setAnimatedSurfaceSize(*reinterpret_cast< QSize*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *Surface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Surface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSurfaceENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Surface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Surface::primaryViewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Surface::geometryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
