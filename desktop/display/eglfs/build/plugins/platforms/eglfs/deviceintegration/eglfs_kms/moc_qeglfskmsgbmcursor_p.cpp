/****************************************************************************
** Meta object code from reading C++ file 'qeglfskmsgbmcursor_p.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../plugins/platforms/eglfs/deviceintegration/eglfs_kms/qeglfskmsgbmcursor_p.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qeglfskmsgbmcursor_p.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS = QtMocHelpers::stringData(
    "QEglFSKmsGbmCursorDeviceListener",
    "onDeviceListChanged",
    "",
    "QInputDeviceManager::DeviceType",
    "type"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS_t {
    uint offsetsAndSizes[10];
    char stringdata0[33];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[32];
    char stringdata4[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS_t qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 32),  // "QEglFSKmsGbmCursorDeviceListener"
        QT_MOC_LITERAL(33, 19),  // "onDeviceListChanged"
        QT_MOC_LITERAL(53, 0),  // ""
        QT_MOC_LITERAL(54, 31),  // "QInputDeviceManager::DeviceType"
        QT_MOC_LITERAL(86, 4)   // "type"
    },
    "QEglFSKmsGbmCursorDeviceListener",
    "onDeviceListChanged",
    "",
    "QInputDeviceManager::DeviceType",
    "type"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject QEglFSKmsGbmCursorDeviceListener::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QEglFSKmsGbmCursorDeviceListener, std::true_type>,
        // method 'onDeviceListChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QInputDeviceManager::DeviceType, std::false_type>
    >,
    nullptr
} };

void QEglFSKmsGbmCursorDeviceListener::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QEglFSKmsGbmCursorDeviceListener *>(_o);
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

const QMetaObject *QEglFSKmsGbmCursorDeviceListener::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QEglFSKmsGbmCursorDeviceListener::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQEglFSKmsGbmCursorDeviceListenerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QEglFSKmsGbmCursorDeviceListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS = QtMocHelpers::stringData(
    "HWEglFSKmsGbmCursor"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS_t qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS = {
    {
        QT_MOC_LITERAL(0, 19)   // "HWEglFSKmsGbmCursor"
    },
    "HWEglFSKmsGbmCursor"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSHWEglFSKmsGbmCursorENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject HWEglFSKmsGbmCursor::staticMetaObject = { {
    QMetaObject::SuperData::link<QPlatformCursor::staticMetaObject>(),
    qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSHWEglFSKmsGbmCursorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<HWEglFSKmsGbmCursor, std::true_type>
    >,
    nullptr
} };

void HWEglFSKmsGbmCursor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *HWEglFSKmsGbmCursor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HWEglFSKmsGbmCursor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSHWEglFSKmsGbmCursorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QPlatformCursor::qt_metacast(_clname);
}

int HWEglFSKmsGbmCursor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlatformCursor::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
