/****************************************************************************
** Meta object code from reading C++ file 'udevmonitor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../eglfs/libhwudev/hollywood/udevmonitor.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udevmonitor.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSQtUdevSCOPEUdevMonitorENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSQtUdevSCOPEUdevMonitorENDCLASS = QtMocHelpers::stringData(
    "QtUdev::UdevMonitor",
    "deviceAdded",
    "",
    "QtUdev::UdevDevice",
    "device",
    "deviceRemoved",
    "deviceChanged",
    "deviceOnlined",
    "deviceOfflined",
    "_q_udevEventHandler"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQtUdevSCOPEUdevMonitorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       5,    1,   53,    2, 0x06,    3 /* Public */,
       6,    1,   56,    2, 0x06,    5 /* Public */,
       7,    1,   59,    2, 0x06,    7 /* Public */,
       8,    1,   62,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   65,    2, 0x08,   11 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject QtUdev::UdevMonitor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSQtUdevSCOPEUdevMonitorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQtUdevSCOPEUdevMonitorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQtUdevSCOPEUdevMonitorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UdevMonitor, std::true_type>,
        // method 'deviceAdded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QtUdev::UdevDevice &, std::false_type>,
        // method 'deviceRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QtUdev::UdevDevice &, std::false_type>,
        // method 'deviceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QtUdev::UdevDevice &, std::false_type>,
        // method 'deviceOnlined'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QtUdev::UdevDevice &, std::false_type>,
        // method 'deviceOfflined'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QtUdev::UdevDevice &, std::false_type>,
        // method '_q_udevEventHandler'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void QtUdev::UdevMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UdevMonitor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->deviceAdded((*reinterpret_cast< std::add_pointer_t<QtUdev::UdevDevice>>(_a[1]))); break;
        case 1: _t->deviceRemoved((*reinterpret_cast< std::add_pointer_t<QtUdev::UdevDevice>>(_a[1]))); break;
        case 2: _t->deviceChanged((*reinterpret_cast< std::add_pointer_t<QtUdev::UdevDevice>>(_a[1]))); break;
        case 3: _t->deviceOnlined((*reinterpret_cast< std::add_pointer_t<QtUdev::UdevDevice>>(_a[1]))); break;
        case 4: _t->deviceOfflined((*reinterpret_cast< std::add_pointer_t<QtUdev::UdevDevice>>(_a[1]))); break;
        case 5: _t->d_func()->_q_udevEventHandler(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UdevMonitor::*)(const QtUdev::UdevDevice & );
            if (_t _q_method = &UdevMonitor::deviceAdded; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UdevMonitor::*)(const QtUdev::UdevDevice & );
            if (_t _q_method = &UdevMonitor::deviceRemoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UdevMonitor::*)(const QtUdev::UdevDevice & );
            if (_t _q_method = &UdevMonitor::deviceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UdevMonitor::*)(const QtUdev::UdevDevice & );
            if (_t _q_method = &UdevMonitor::deviceOnlined; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (UdevMonitor::*)(const QtUdev::UdevDevice & );
            if (_t _q_method = &UdevMonitor::deviceOfflined; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *QtUdev::UdevMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtUdev::UdevMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQtUdevSCOPEUdevMonitorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QtUdev::UdevMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QtUdev::UdevMonitor::deviceAdded(const QtUdev::UdevDevice & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtUdev::UdevMonitor::deviceRemoved(const QtUdev::UdevDevice & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtUdev::UdevMonitor::deviceChanged(const QtUdev::UdevDevice & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtUdev::UdevMonitor::deviceOnlined(const QtUdev::UdevDevice & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QtUdev::UdevMonitor::deviceOfflined(const QtUdev::UdevDevice & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
