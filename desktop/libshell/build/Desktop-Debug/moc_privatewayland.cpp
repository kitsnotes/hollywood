/****************************************************************************
** Meta object code from reading C++ file 'privatewayland.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/privatewayland.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'privatewayland.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSHWPrivateWaylandProtocolENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSHWPrivateWaylandProtocolENDCLASS = QtMocHelpers::stringData(
    "HWPrivateWaylandProtocol"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSHWPrivateWaylandProtocolENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject HWPrivateWaylandProtocol::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandClientExtensionTemplate<HWPrivateWaylandProtocol>::staticMetaObject>(),
    qt_meta_stringdata_CLASSHWPrivateWaylandProtocolENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSHWPrivateWaylandProtocolENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSHWPrivateWaylandProtocolENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HWPrivateWaylandProtocol, std::true_type>
    >,
    nullptr
} };

void HWPrivateWaylandProtocol::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *HWPrivateWaylandProtocol::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HWPrivateWaylandProtocol::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSHWPrivateWaylandProtocolENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWayland::org_originull_privateapi"))
        return static_cast< QtWayland::org_originull_privateapi*>(this);
    return QWaylandClientExtensionTemplate<HWPrivateWaylandProtocol>::qt_metacast(_clname);
}

int HWPrivateWaylandProtocol::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandClientExtensionTemplate<HWPrivateWaylandProtocol>::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSOriginullMenuServerClientENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSOriginullMenuServerClientENDCLASS = QtMocHelpers::stringData(
    "OriginullMenuServerClient",
    "menuChanged",
    "",
    "serviceName",
    "objectPath"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSOriginullMenuServerClientENDCLASS[] = {

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
       1,    2,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,

       0        // eod
};

Q_CONSTINIT const QMetaObject OriginullMenuServerClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandClientExtensionTemplate<OriginullMenuServerClient>::staticMetaObject>(),
    qt_meta_stringdata_CLASSOriginullMenuServerClientENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSOriginullMenuServerClientENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSOriginullMenuServerClientENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OriginullMenuServerClient, std::true_type>,
        // method 'menuChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void OriginullMenuServerClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OriginullMenuServerClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->menuChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OriginullMenuServerClient::*)(const QString & , const QString & );
            if (_t _q_method = &OriginullMenuServerClient::menuChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *OriginullMenuServerClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OriginullMenuServerClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSOriginullMenuServerClientENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWayland::org_originull_menuserver"))
        return static_cast< QtWayland::org_originull_menuserver*>(this);
    return QWaylandClientExtensionTemplate<OriginullMenuServerClient>::qt_metacast(_clname);
}

int OriginullMenuServerClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandClientExtensionTemplate<OriginullMenuServerClient>::qt_metacall(_c, _id, _a);
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
void OriginullMenuServerClient::menuChanged(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
