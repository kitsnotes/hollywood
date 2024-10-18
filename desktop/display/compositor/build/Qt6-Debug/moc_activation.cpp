/****************************************************************************
** Meta object code from reading C++ file 'activation.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/activation.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'activation.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSXdgActivationENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSXdgActivationENDCLASS = QtMocHelpers::stringData(
    "XdgActivation",
    "surfaceActivated",
    "",
    "QWaylandSurface*",
    "surface"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSXdgActivationENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

Q_CONSTINIT const QMetaObject XdgActivation::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandCompositorExtensionTemplate<XdgActivation>::staticMetaObject>(),
    qt_meta_stringdata_CLASSXdgActivationENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSXdgActivationENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSXdgActivationENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<XdgActivation, std::true_type>,
        // method 'surfaceActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWaylandSurface *, std::false_type>
    >,
    nullptr
} };

void XdgActivation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<XdgActivation *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->surfaceActivated((*reinterpret_cast< std::add_pointer_t<QWaylandSurface*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (XdgActivation::*)(QWaylandSurface * );
            if (_t _q_method = &XdgActivation::surfaceActivated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *XdgActivation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *XdgActivation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSXdgActivationENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWaylandServer::xdg_activation_v1"))
        return static_cast< QtWaylandServer::xdg_activation_v1*>(this);
    return QWaylandCompositorExtensionTemplate<XdgActivation>::qt_metacast(_clname);
}

int XdgActivation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandCompositorExtensionTemplate<XdgActivation>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void XdgActivation::surfaceActivated(QWaylandSurface * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSXdgActivationTokenENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSXdgActivationTokenENDCLASS = QtMocHelpers::stringData(
    "XdgActivationToken"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSXdgActivationTokenENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject XdgActivationToken::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandCompositorExtensionTemplate<XdgActivationToken>::staticMetaObject>(),
    qt_meta_stringdata_CLASSXdgActivationTokenENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSXdgActivationTokenENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSXdgActivationTokenENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<XdgActivationToken, std::true_type>
    >,
    nullptr
} };

void XdgActivationToken::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *XdgActivationToken::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *XdgActivationToken::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSXdgActivationTokenENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWaylandServer::xdg_activation_token_v1"))
        return static_cast< QtWaylandServer::xdg_activation_token_v1*>(this);
    return QWaylandCompositorExtensionTemplate<XdgActivationToken>::qt_metacast(_clname);
}

int XdgActivationToken::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandCompositorExtensionTemplate<XdgActivationToken>::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
