/****************************************************************************
** Meta object code from reading C++ file 'qeglfscursor_p.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugins/platforms/eglfs/api/hollywood/private/qeglfscursor_p.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qeglfscursor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.2. It"
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
struct qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS = QtMocHelpers::stringData(
    "QEglFSCursorDeviceListener",
    "onDeviceListChanged",
    "",
    "QInputDeviceManager::DeviceType",
    "type"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS_t {
    uint offsetsAndSizes[10];
    char stringdata0[27];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[32];
    char stringdata4[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS_t qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 26),  // "QEglFSCursorDeviceListener"
        QT_MOC_LITERAL(27, 19),  // "onDeviceListChanged"
        QT_MOC_LITERAL(47, 0),  // ""
        QT_MOC_LITERAL(48, 31),  // "QInputDeviceManager::DeviceType"
        QT_MOC_LITERAL(80, 4)   // "type"
    },
    "QEglFSCursorDeviceListener",
    "onDeviceListChanged",
    "",
    "QInputDeviceManager::DeviceType",
    "type"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQEglFSCursorDeviceListenerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x0a,    1 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

Q_CONSTINIT const QMetaObject QEglFSCursorDeviceListener::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQEglFSCursorDeviceListenerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QEglFSCursorDeviceListener, std::true_type>,
        // method 'onDeviceListChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QInputDeviceManager::DeviceType, std::false_type>
    >,
    nullptr
} };

void QEglFSCursorDeviceListener::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QEglFSCursorDeviceListener *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onDeviceListChanged((*reinterpret_cast< std::add_pointer_t<QInputDeviceManager::DeviceType>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QInputDeviceManager::DeviceType >(); break;
            }
            break;
        }
    }
}

const QMetaObject *QEglFSCursorDeviceListener::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QEglFSCursorDeviceListener::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQEglFSCursorDeviceListenerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QEglFSCursorDeviceListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSQEglFSCursorENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQEglFSCursorENDCLASS = QtMocHelpers::stringData(
    "QEglFSCursor"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQEglFSCursorENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQEglFSCursorENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQEglFSCursorENDCLASS_t qt_meta_stringdata_CLASSQEglFSCursorENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12)   // "QEglFSCursor"
    },
    "QEglFSCursor"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQEglFSCursorENDCLASS[] = {

 // content:
      11,       // revision
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

Q_CONSTINIT const QMetaObject QEglFSCursor::staticMetaObject = { {
    QMetaObject::SuperData::link<QPlatformCursor::staticMetaObject>(),
    qt_meta_stringdata_CLASSQEglFSCursorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQEglFSCursorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQEglFSCursorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QEglFSCursor, std::true_type>
    >,
    nullptr
} };

void QEglFSCursor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *QEglFSCursor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QEglFSCursor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQEglFSCursorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions"))
        return static_cast< QOpenGLFunctions*>(this);
    return QPlatformCursor::qt_metacast(_clname);
}

int QEglFSCursor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlatformCursor::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
