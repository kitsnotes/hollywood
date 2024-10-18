/****************************************************************************
** Meta object code from reading C++ file 'xdgshell.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/xdgshell.h"
#include "qwaylandsurface.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xdgshell.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSHWWaylandXdgShellENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSHWWaylandXdgShellENDCLASS = QtMocHelpers::stringData(
    "HWWaylandXdgShell",
    "xdgSurfaceCreated",
    "",
    "HWWaylandXdgSurface*",
    "xdgSurface",
    "toplevelCreated",
    "HWWaylandXdgToplevel*",
    "toplevel",
    "popupCreated",
    "HWWaylandXdgPopup*",
    "popup",
    "pong",
    "serial",
    "ping",
    "QWaylandClient*",
    "client",
    "handleSeatChanged",
    "QWaylandSeat*",
    "newSeat",
    "oldSeat",
    "handleFocusChanged",
    "QWaylandSurface*",
    "newSurface",
    "oldSurface"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSHWWaylandXdgShellENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,    1 /* Public */,
       5,    2,   59,    2, 0x06,    3 /* Public */,
       8,    2,   64,    2, 0x06,    6 /* Public */,
      11,    1,   69,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    1,   72,    2, 0x0a,   11 /* Public */,
      16,    2,   75,    2, 0x08,   13 /* Private */,
      20,    2,   80,    2, 0x08,   16 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 3,    7,    4,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 3,   10,    4,
    QMetaType::Void, QMetaType::UInt,   12,

 // slots: parameters
    QMetaType::UInt, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 17,   18,   19,
    QMetaType::Void, 0x80000000 | 21, 0x80000000 | 21,   22,   23,

       0        // eod
};

Q_CONSTINIT const QMetaObject HWWaylandXdgShell::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandShellTemplate<HWWaylandXdgShell>::staticMetaObject>(),
    qt_meta_stringdata_CLASSHWWaylandXdgShellENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSHWWaylandXdgShellENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSHWWaylandXdgShellENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HWWaylandXdgShell, std::true_type>,
        // method 'xdgSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface *, std::false_type>,
        // method 'toplevelCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel *, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface *, std::false_type>,
        // method 'popupCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgPopup *, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface *, std::false_type>,
        // method 'pong'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'ping'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandClient *, std::false_type>,
        // method 'handleSeatChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        // method 'handleFocusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>
    >,
    nullptr
} };

void HWWaylandXdgShell::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HWWaylandXdgShell *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->xdgSurfaceCreated((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgSurface*>>(_a[1]))); break;
        case 1: _t->toplevelCreated((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgToplevel*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<HWWaylandXdgSurface*>>(_a[2]))); break;
        case 2: _t->popupCreated((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgPopup*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<HWWaylandXdgSurface*>>(_a[2]))); break;
        case 3: _t->pong((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1]))); break;
        case 4: { uint _r = _t->ping((*reinterpret_cast< std::add_pointer_t<QWaylandClient*>>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->handleSeatChanged((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[2]))); break;
        case 6: _t->handleFocusChanged((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgSurface* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgSurface* >(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgToplevel* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgPopup* >(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgSurface* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandClient* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HWWaylandXdgShell::*)(HWWaylandXdgSurface * );
            if (_t _q_method = &HWWaylandXdgShell::xdgSurfaceCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgShell::*)(HWWaylandXdgToplevel * , HWWaylandXdgSurface * );
            if (_t _q_method = &HWWaylandXdgShell::toplevelCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgShell::*)(HWWaylandXdgPopup * , HWWaylandXdgSurface * );
            if (_t _q_method = &HWWaylandXdgShell::popupCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgShell::*)(uint );
            if (_t _q_method = &HWWaylandXdgShell::pong; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *HWWaylandXdgShell::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HWWaylandXdgShell::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSHWWaylandXdgShellENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWaylandShellTemplate<HWWaylandXdgShell>::qt_metacast(_clname);
}

int HWWaylandXdgShell::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandShellTemplate<HWWaylandXdgShell>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void HWWaylandXdgShell::xdgSurfaceCreated(HWWaylandXdgSurface * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HWWaylandXdgShell::toplevelCreated(HWWaylandXdgToplevel * _t1, HWWaylandXdgSurface * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HWWaylandXdgShell::popupCreated(HWWaylandXdgPopup * _t1, HWWaylandXdgSurface * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void HWWaylandXdgShell::pong(uint _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSHWWaylandXdgSurfaceENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSHWWaylandXdgSurfaceENDCLASS = QtMocHelpers::stringData(
    "HWWaylandXdgSurface",
    "DefaultProperty",
    "data",
    "shellChanged",
    "",
    "surfaceChanged",
    "toplevelCreated",
    "popupCreated",
    "windowGeometryChanged",
    "handleSurfaceSizeChanged",
    "handleBufferScaleChanged",
    "initialize",
    "HWWaylandXdgShell*",
    "xdgShell",
    "QWaylandSurface*",
    "surface",
    "QWaylandResource",
    "resource",
    "QQmlListProperty<QObject>",
    "shell",
    "toplevel",
    "HWWaylandXdgToplevel*",
    "popup",
    "HWWaylandXdgPopup*",
    "windowGeometry"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSHWWaylandXdgSurfaceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       1,   14, // classinfo
       8,   16, // methods
       6,   78, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // classinfo: key, value
       1,    2,

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   64,    4, 0x06,    7 /* Public */,
       5,    0,   65,    4, 0x06,    8 /* Public */,
       6,    0,   66,    4, 0x06,    9 /* Public */,
       7,    0,   67,    4, 0x06,   10 /* Public */,
       8,    0,   68,    4, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   69,    4, 0x08,   12 /* Private */,
      10,    0,   70,    4, 0x08,   13 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      11,    3,   71,    4, 0x02,   14 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, 0x80000000 | 16,   13,   15,   17,

 // properties: name, type, flags, notifyId, revision
       2, 0x80000000 | 18, 0x00014009, uint(-1), 0,
      19, 0x80000000 | 12, 0x00015009, uint(0), 0,
      15, 0x80000000 | 14, 0x00015009, uint(1), 0,
      20, 0x80000000 | 21, 0x00015009, uint(2), 0,
      22, 0x80000000 | 23, 0x00015009, uint(3), 0,
      24, QMetaType::QRect, 0x00015001, uint(4), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject HWWaylandXdgSurface::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>::staticMetaObject>(),
    qt_meta_stringdata_CLASSHWWaylandXdgSurfaceENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSHWWaylandXdgSurfaceENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSHWWaylandXdgSurfaceENDCLASS_t,
        // property 'data'
        QtPrivate::TypeAndForceComplete<QQmlListProperty<QObject>, std::true_type>,
        // property 'shell'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgShell*, std::true_type>,
        // property 'surface'
        QtPrivate::TypeAndForceComplete<QWaylandSurface*, std::true_type>,
        // property 'toplevel'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel*, std::true_type>,
        // property 'popup'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgPopup*, std::true_type>,
        // property 'windowGeometry'
        QtPrivate::TypeAndForceComplete<QRect, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface, std::true_type>,
        // method 'shellChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'surfaceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toplevelCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'popupCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'windowGeometryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleSurfaceSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleBufferScaleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'initialize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgShell *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QWaylandResource &, std::false_type>
    >,
    nullptr
} };

void HWWaylandXdgSurface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HWWaylandXdgSurface *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->shellChanged(); break;
        case 1: _t->surfaceChanged(); break;
        case 2: _t->toplevelCreated(); break;
        case 3: _t->popupCreated(); break;
        case 4: _t->windowGeometryChanged(); break;
        case 5: _t->handleSurfaceSizeChanged(); break;
        case 6: _t->handleBufferScaleChanged(); break;
        case 7: _t->initialize((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgShell*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QWaylandResource>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgShell* >(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandResource >(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandSurface* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HWWaylandXdgSurface::*)();
            if (_t _q_method = &HWWaylandXdgSurface::shellChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgSurface::*)();
            if (_t _q_method = &HWWaylandXdgSurface::surfaceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgSurface::*)();
            if (_t _q_method = &HWWaylandXdgSurface::toplevelCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgSurface::*)();
            if (_t _q_method = &HWWaylandXdgSurface::popupCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgSurface::*)();
            if (_t _q_method = &HWWaylandXdgSurface::windowGeometryChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HWWaylandXdgPopup* >(); break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HWWaylandXdgShell* >(); break;
        case 3:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HWWaylandXdgToplevel* >(); break;
        case 2:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWaylandSurface* >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<HWWaylandXdgSurface *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QQmlListProperty<QObject>*>(_v) = _t->data(); break;
        case 1: *reinterpret_cast< HWWaylandXdgShell**>(_v) = _t->shell(); break;
        case 2: *reinterpret_cast< QWaylandSurface**>(_v) = _t->surface(); break;
        case 3: *reinterpret_cast< HWWaylandXdgToplevel**>(_v) = _t->toplevel(); break;
        case 4: *reinterpret_cast< HWWaylandXdgPopup**>(_v) = _t->popup(); break;
        case 5: *reinterpret_cast< QRect*>(_v) = _t->windowGeometry(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *HWWaylandXdgSurface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HWWaylandXdgSurface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSHWWaylandXdgSurfaceENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>::qt_metacast(_clname);
}

int HWWaylandXdgSurface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void HWWaylandXdgSurface::shellChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HWWaylandXdgSurface::surfaceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HWWaylandXdgSurface::toplevelCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void HWWaylandXdgSurface::popupCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void HWWaylandXdgSurface::windowGeometryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSHWWaylandXdgToplevelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSHWWaylandXdgToplevelENDCLASS = QtMocHelpers::stringData(
    "HWWaylandXdgToplevel",
    "parentToplevelChanged",
    "",
    "titleChanged",
    "appIdChanged",
    "maxSizeChanged",
    "minSizeChanged",
    "startMove",
    "QWaylandSeat*",
    "seat",
    "startResize",
    "Qt::Edges",
    "edges",
    "statesChanged",
    "maximizedChanged",
    "fullscreenChanged",
    "resizingChanged",
    "activatedChanged",
    "showWindowMenu",
    "localSurfacePosition",
    "setMaximized",
    "unsetMaximized",
    "setFullscreen",
    "QWaylandOutput*",
    "output",
    "unsetFullscreen",
    "setMinimized",
    "decorationModeChanged",
    "modalChanged",
    "sizeForResize",
    "size",
    "delta",
    "sendConfigure",
    "QList<int>",
    "states",
    "sendClose",
    "sendMaximized",
    "sendUnmaximized",
    "sendFullscreen",
    "sendResizing",
    "maxSize",
    "xdgSurface",
    "HWWaylandXdgSurface*",
    "parentToplevel",
    "HWWaylandXdgToplevel*",
    "title",
    "appId",
    "minSize",
    "maximized",
    "fullscreen",
    "resizing",
    "activated",
    "modal",
    "decorationMode",
    "DecorationMode",
    "State",
    "MaximizedState",
    "FullscreenState",
    "ResizingState",
    "ActivatedState",
    "ClientSideDecoration",
    "ServerSideDecoration"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSHWWaylandXdgToplevelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
      13,  268, // properties
       2,  333, // enums/sets
       0,    0, // constructors
       0,       // flags
      20,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  210,    2, 0x06,   16 /* Public */,
       3,    0,  211,    2, 0x06,   17 /* Public */,
       4,    0,  212,    2, 0x06,   18 /* Public */,
       5,    0,  213,    2, 0x06,   19 /* Public */,
       6,    0,  214,    2, 0x06,   20 /* Public */,
       7,    1,  215,    2, 0x06,   21 /* Public */,
      10,    2,  218,    2, 0x06,   23 /* Public */,
      13,    0,  223,    2, 0x06,   26 /* Public */,
      14,    0,  224,    2, 0x06,   27 /* Public */,
      15,    0,  225,    2, 0x06,   28 /* Public */,
      16,    0,  226,    2, 0x06,   29 /* Public */,
      17,    0,  227,    2, 0x06,   30 /* Public */,
      18,    2,  228,    2, 0x06,   31 /* Public */,
      20,    0,  233,    2, 0x06,   34 /* Public */,
      21,    0,  234,    2, 0x06,   35 /* Public */,
      22,    1,  235,    2, 0x06,   36 /* Public */,
      25,    0,  238,    2, 0x06,   38 /* Public */,
      26,    0,  239,    2, 0x06,   39 /* Public */,
      27,    0,  240,    2, 0x06,   40 /* Public */,
      28,    0,  241,    2, 0x86,   41 /* Public | MethodRevisioned */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      29,    3,  242,    2, 0x102,   42 /* Public | MethodIsConst  */,
      32,    2,  249,    2, 0x02,   46 /* Public */,
      35,    0,  254,    2, 0x02,   49 /* Public */,
      36,    1,  255,    2, 0x02,   50 /* Public */,
      37,    1,  258,    2, 0x02,   52 /* Public */,
      37,    0,  261,    2, 0x22,   54 /* Public | MethodCloned */,
      38,    1,  262,    2, 0x02,   55 /* Public */,
      39,    1,  265,    2, 0x02,   57 /* Public */,

 // signals: revision
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
    1544,

 // methods: revision
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 11,    9,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QPoint,    9,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::QSize, QMetaType::QSizeF, QMetaType::QPointF, 0x80000000 | 11,   30,   31,   12,
    QMetaType::UInt, QMetaType::QSize, 0x80000000 | 33,   30,   34,
    QMetaType::Void,
    QMetaType::UInt, QMetaType::QSize,   30,
    QMetaType::UInt, QMetaType::QSize,   30,
    QMetaType::UInt,
    QMetaType::UInt, QMetaType::QSize,   30,
    QMetaType::UInt, QMetaType::QSize,   40,

 // properties: name, type, flags, notifyId, revision
      41, 0x80000000 | 42, 0x00015409, uint(-1), 0,
      43, 0x80000000 | 44, 0x00015009, uint(0), 0,
      45, QMetaType::QString, 0x00015001, uint(1), 0,
      46, QMetaType::QString, 0x00015001, uint(2), 0,
      40, QMetaType::QSize, 0x00015001, uint(3), 0,
      47, QMetaType::QSize, 0x00015001, uint(4), 0,
      34, 0x80000000 | 33, 0x00015009, uint(7), 0,
      48, QMetaType::Bool, 0x00015001, uint(8), 0,
      49, QMetaType::Bool, 0x00015001, uint(9), 0,
      50, QMetaType::Bool, 0x00015001, uint(10), 0,
      51, QMetaType::Bool, 0x00015001, uint(11), 0,
      52, QMetaType::Bool, 0x00015801, uint(19), 1544,
      53, 0x80000000 | 54, 0x00015009, uint(18), 0,

 // enums: name, alias, flags, count, data
      55,   55, 0x0,    4,  343,
      54,   54, 0x0,    2,  351,

 // enum data: key, value
      56, uint(HWWaylandXdgToplevel::MaximizedState),
      57, uint(HWWaylandXdgToplevel::FullscreenState),
      58, uint(HWWaylandXdgToplevel::ResizingState),
      59, uint(HWWaylandXdgToplevel::ActivatedState),
      60, uint(HWWaylandXdgToplevel::ClientSideDecoration),
      61, uint(HWWaylandXdgToplevel::ServerSideDecoration),

       0        // eod
};

Q_CONSTINIT const QMetaObject HWWaylandXdgToplevel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSHWWaylandXdgToplevelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSHWWaylandXdgToplevelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSHWWaylandXdgToplevelENDCLASS_t,
        // property 'xdgSurface'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface*, std::true_type>,
        // property 'parentToplevel'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel*, std::true_type>,
        // property 'title'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'appId'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'maxSize'
        QtPrivate::TypeAndForceComplete<QSize, std::true_type>,
        // property 'minSize'
        QtPrivate::TypeAndForceComplete<QSize, std::true_type>,
        // property 'states'
        QtPrivate::TypeAndForceComplete<QList<int>, std::true_type>,
        // property 'maximized'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'fullscreen'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'resizing'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'activated'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'modal'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'decorationMode'
        QtPrivate::TypeAndForceComplete<DecorationMode, std::true_type>,
        // enum 'State'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel::State, std::true_type>,
        // enum 'DecorationMode'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel::DecorationMode, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HWWaylandXdgToplevel, std::true_type>,
        // method 'parentToplevelChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'titleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'appIdChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'maxSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'minSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        // method 'startResize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::Edges, std::false_type>,
        // method 'statesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'maximizedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fullscreenChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resizingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activatedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showWindowMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSeat *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'setMaximized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unsetMaximized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setFullscreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandOutput *, std::false_type>,
        // method 'unsetFullscreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setMinimized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'decorationModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'modalChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sizeForResize'
        QtPrivate::TypeAndForceComplete<QSize, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSizeF &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPointF &, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::Edges, std::false_type>,
        // method 'sendConfigure'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QList<int> &, std::false_type>,
        // method 'sendClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendMaximized'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        // method 'sendUnmaximized'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        // method 'sendUnmaximized'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'sendFullscreen'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        // method 'sendResizing'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>
    >,
    nullptr
} };

void HWWaylandXdgToplevel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HWWaylandXdgToplevel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->parentToplevelChanged(); break;
        case 1: _t->titleChanged(); break;
        case 2: _t->appIdChanged(); break;
        case 3: _t->maxSizeChanged(); break;
        case 4: _t->minSizeChanged(); break;
        case 5: _t->startMove((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1]))); break;
        case 6: _t->startResize((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Qt::Edges>>(_a[2]))); break;
        case 7: _t->statesChanged(); break;
        case 8: _t->maximizedChanged(); break;
        case 9: _t->fullscreenChanged(); break;
        case 10: _t->resizingChanged(); break;
        case 11: _t->activatedChanged(); break;
        case 12: _t->showWindowMenu((*reinterpret_cast< std::add_pointer_t<QWaylandSeat*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 13: _t->setMaximized(); break;
        case 14: _t->unsetMaximized(); break;
        case 15: _t->setFullscreen((*reinterpret_cast< std::add_pointer_t<QWaylandOutput*>>(_a[1]))); break;
        case 16: _t->unsetFullscreen(); break;
        case 17: _t->setMinimized(); break;
        case 18: _t->decorationModeChanged(); break;
        case 19: _t->modalChanged(); break;
        case 20: { QSize _r = _t->sizeForResize((*reinterpret_cast< std::add_pointer_t<QSizeF>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<Qt::Edges>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QSize*>(_a[0]) = std::move(_r); }  break;
        case 21: { uint _r = _t->sendConfigure((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[2])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 22: _t->sendClose(); break;
        case 23: { uint _r = _t->sendMaximized((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 24: { uint _r = _t->sendUnmaximized((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 25: { uint _r = _t->sendUnmaximized();
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 26: { uint _r = _t->sendFullscreen((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 27: { uint _r = _t->sendResizing((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWaylandOutput* >(); break;
            }
            break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::parentToplevelChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::titleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::appIdChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::maxSizeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::minSizeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)(QWaylandSeat * );
            if (_t _q_method = &HWWaylandXdgToplevel::startMove; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)(QWaylandSeat * , Qt::Edges );
            if (_t _q_method = &HWWaylandXdgToplevel::startResize; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::statesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::maximizedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::fullscreenChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::resizingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::activatedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)(QWaylandSeat * , const QPoint & );
            if (_t _q_method = &HWWaylandXdgToplevel::showWindowMenu; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::setMaximized; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::unsetMaximized; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)(QWaylandOutput * );
            if (_t _q_method = &HWWaylandXdgToplevel::setFullscreen; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::unsetFullscreen; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::setMinimized; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::decorationModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (HWWaylandXdgToplevel::*)();
            if (_t _q_method = &HWWaylandXdgToplevel::modalChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 19;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HWWaylandXdgSurface* >(); break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HWWaylandXdgToplevel* >(); break;
        case 6:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<HWWaylandXdgToplevel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< HWWaylandXdgSurface**>(_v) = _t->xdgSurface(); break;
        case 1: *reinterpret_cast< HWWaylandXdgToplevel**>(_v) = _t->parentToplevel(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->title(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->appId(); break;
        case 4: *reinterpret_cast< QSize*>(_v) = _t->maxSize(); break;
        case 5: *reinterpret_cast< QSize*>(_v) = _t->minSize(); break;
        case 6: *reinterpret_cast< QList<int>*>(_v) = _t->statesAsInts(); break;
        case 7: *reinterpret_cast< bool*>(_v) = _t->maximized(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->fullscreen(); break;
        case 9: *reinterpret_cast< bool*>(_v) = _t->resizing(); break;
        case 10: *reinterpret_cast< bool*>(_v) = _t->activated(); break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->isModal(); break;
        case 12: *reinterpret_cast< DecorationMode*>(_v) = _t->decorationMode(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *HWWaylandXdgToplevel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HWWaylandXdgToplevel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSHWWaylandXdgToplevelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HWWaylandXdgToplevel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void HWWaylandXdgToplevel::parentToplevelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HWWaylandXdgToplevel::titleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HWWaylandXdgToplevel::appIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void HWWaylandXdgToplevel::maxSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void HWWaylandXdgToplevel::minSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void HWWaylandXdgToplevel::startMove(QWaylandSeat * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void HWWaylandXdgToplevel::startResize(QWaylandSeat * _t1, Qt::Edges _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void HWWaylandXdgToplevel::statesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void HWWaylandXdgToplevel::maximizedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void HWWaylandXdgToplevel::fullscreenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void HWWaylandXdgToplevel::resizingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void HWWaylandXdgToplevel::activatedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void HWWaylandXdgToplevel::showWindowMenu(QWaylandSeat * _t1, const QPoint & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void HWWaylandXdgToplevel::setMaximized()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void HWWaylandXdgToplevel::unsetMaximized()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void HWWaylandXdgToplevel::setFullscreen(QWaylandOutput * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void HWWaylandXdgToplevel::unsetFullscreen()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void HWWaylandXdgToplevel::setMinimized()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void HWWaylandXdgToplevel::decorationModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void HWWaylandXdgToplevel::modalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSHWWaylandXdgPopupENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSHWWaylandXdgPopupENDCLASS = QtMocHelpers::stringData(
    "HWWaylandXdgPopup",
    "configuredGeometryChanged",
    "",
    "sendConfigure",
    "geometry",
    "sendPopupDone",
    "xdgSurface",
    "HWWaylandXdgSurface*",
    "parentXdgSurface",
    "configuredGeometry",
    "anchorRect",
    "anchorEdges",
    "Qt::Edges",
    "gravityEdges",
    "slideConstraints",
    "Qt::Orientations",
    "flipConstraints",
    "resizeConstraints",
    "offset",
    "positionerSize",
    "unconstrainedPosition"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSHWWaylandXdgPopupENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
      12,   40, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   35,    2, 0x06,   13 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   36,    2, 0x02,   14 /* Public */,
       5,    0,   39,    2, 0x82,   16 /* Public | MethodRevisioned */,

 // signals: revision
       0,

 // methods: revision
       0,
     270,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::UInt, QMetaType::QRect,    4,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
       6, 0x80000000 | 7, 0x00015409, uint(-1), 0,
       8, 0x80000000 | 7, 0x00015409, uint(-1), 0,
       9, QMetaType::QRect, 0x00015001, uint(0), 0,
      10, QMetaType::QRect, 0x00015401, uint(-1), 0,
      11, 0x80000000 | 12, 0x00015409, uint(-1), 0,
      13, 0x80000000 | 12, 0x00015409, uint(-1), 0,
      14, 0x80000000 | 15, 0x00015409, uint(-1), 0,
      16, 0x80000000 | 15, 0x00015409, uint(-1), 0,
      17, 0x80000000 | 15, 0x00015409, uint(-1), 0,
      18, QMetaType::QPoint, 0x00015401, uint(-1), 0,
      19, QMetaType::QSize, 0x00015401, uint(-1), 0,
      20, QMetaType::QPoint, 0x00015401, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject HWWaylandXdgPopup::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSHWWaylandXdgPopupENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSHWWaylandXdgPopupENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSHWWaylandXdgPopupENDCLASS_t,
        // property 'xdgSurface'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface*, std::true_type>,
        // property 'parentXdgSurface'
        QtPrivate::TypeAndForceComplete<HWWaylandXdgSurface*, std::true_type>,
        // property 'configuredGeometry'
        QtPrivate::TypeAndForceComplete<QRect, std::true_type>,
        // property 'anchorRect'
        QtPrivate::TypeAndForceComplete<QRect, std::true_type>,
        // property 'anchorEdges'
        QtPrivate::TypeAndForceComplete<Qt::Edges, std::true_type>,
        // property 'gravityEdges'
        QtPrivate::TypeAndForceComplete<Qt::Edges, std::true_type>,
        // property 'slideConstraints'
        QtPrivate::TypeAndForceComplete<Qt::Orientations, std::true_type>,
        // property 'flipConstraints'
        QtPrivate::TypeAndForceComplete<Qt::Orientations, std::true_type>,
        // property 'resizeConstraints'
        QtPrivate::TypeAndForceComplete<Qt::Orientations, std::true_type>,
        // property 'offset'
        QtPrivate::TypeAndForceComplete<QPoint, std::true_type>,
        // property 'positionerSize'
        QtPrivate::TypeAndForceComplete<QSize, std::true_type>,
        // property 'unconstrainedPosition'
        QtPrivate::TypeAndForceComplete<QPoint, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HWWaylandXdgPopup, std::true_type>,
        // method 'configuredGeometryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendConfigure'
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QRect &, std::false_type>,
        // method 'sendPopupDone'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void HWWaylandXdgPopup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HWWaylandXdgPopup *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->configuredGeometryChanged(); break;
        case 1: { uint _r = _t->sendConfigure((*reinterpret_cast< std::add_pointer_t<QRect>>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->sendPopupDone(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HWWaylandXdgPopup::*)();
            if (_t _q_method = &HWWaylandXdgPopup::configuredGeometryChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< HWWaylandXdgSurface* >(); break;
        }
    }  else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<HWWaylandXdgPopup *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< HWWaylandXdgSurface**>(_v) = _t->xdgSurface(); break;
        case 1: *reinterpret_cast< HWWaylandXdgSurface**>(_v) = _t->parentXdgSurface(); break;
        case 2: *reinterpret_cast< QRect*>(_v) = _t->configuredGeometry(); break;
        case 3: *reinterpret_cast< QRect*>(_v) = _t->anchorRect(); break;
        case 4: *reinterpret_cast< Qt::Edges*>(_v) = _t->anchorEdges(); break;
        case 5: *reinterpret_cast< Qt::Edges*>(_v) = _t->gravityEdges(); break;
        case 6: *reinterpret_cast< Qt::Orientations*>(_v) = _t->slideConstraints(); break;
        case 7: *reinterpret_cast< Qt::Orientations*>(_v) = _t->flipConstraints(); break;
        case 8: *reinterpret_cast< Qt::Orientations*>(_v) = _t->resizeConstraints(); break;
        case 9: *reinterpret_cast< QPoint*>(_v) = _t->offset(); break;
        case 10: *reinterpret_cast< QSize*>(_v) = _t->positionerSize(); break;
        case 11: *reinterpret_cast< QPoint*>(_v) = _t->unconstrainedPosition(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *HWWaylandXdgPopup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HWWaylandXdgPopup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSHWWaylandXdgPopupENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HWWaylandXdgPopup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void HWWaylandXdgPopup::configuredGeometryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
