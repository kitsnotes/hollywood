/****************************************************************************
** Meta object code from reading C++ file 'logind.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../eglfs/libhwlogind/hollywood/logind.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'logind.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSOriginullSCOPELogindENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSOriginullSCOPELogindENDCLASS = QtMocHelpers::stringData(
    "Originull::Logind",
    "connectedChanged",
    "",
    "hasSessionControlChanged",
    "sessionActiveChanged",
    "inhibitedChanged",
    "vtNumberChanged",
    "seatChanged",
    "seat",
    "prepareForSleep",
    "before",
    "prepareForShutdown",
    "lockSessionRequested",
    "unlockSessionRequested",
    "inhibited",
    "who",
    "why",
    "fd",
    "uninhibited",
    "devicePaused",
    "major",
    "minor",
    "type",
    "deviceResumed",
    "inhibit",
    "Logind::InhibitFlags",
    "flags",
    "Logind::InhibitMode",
    "mode",
    "uninhibit",
    "lockSession",
    "unlockSession",
    "takeControl",
    "releaseControl",
    "takeDevice",
    "fileName",
    "releaseDevice",
    "pauseDeviceComplete",
    "devMajor",
    "devMinor",
    "switchTo",
    "vt",
    "_q_serviceRegistered",
    "_q_serviceUnregistered",
    "_q_sessionPropertiesChanged",
    "connected",
    "hasSessionControl",
    "sessionActive",
    "vtNumber"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSOriginullSCOPELogindENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       6,  259, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  176,    2, 0x06,    7 /* Public */,
       3,    1,  179,    2, 0x06,    9 /* Public */,
       4,    1,  182,    2, 0x06,   11 /* Public */,
       5,    1,  185,    2, 0x06,   13 /* Public */,
       6,    1,  188,    2, 0x06,   15 /* Public */,
       7,    1,  191,    2, 0x06,   17 /* Public */,
       9,    1,  194,    2, 0x06,   19 /* Public */,
      11,    1,  197,    2, 0x06,   21 /* Public */,
      12,    0,  200,    2, 0x06,   23 /* Public */,
      13,    0,  201,    2, 0x06,   24 /* Public */,
      14,    3,  202,    2, 0x06,   25 /* Public */,
      18,    1,  209,    2, 0x06,   29 /* Public */,
      19,    3,  212,    2, 0x06,   31 /* Public */,
      23,    3,  219,    2, 0x06,   35 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      24,    4,  226,    2, 0x0a,   39 /* Public */,
      29,    1,  235,    2, 0x0a,   44 /* Public */,
      30,    0,  238,    2, 0x0a,   46 /* Public */,
      31,    0,  239,    2, 0x0a,   47 /* Public */,
      32,    0,  240,    2, 0x0a,   48 /* Public */,
      33,    0,  241,    2, 0x0a,   49 /* Public */,
      34,    1,  242,    2, 0x0a,   50 /* Public */,
      36,    1,  245,    2, 0x0a,   52 /* Public */,
      37,    2,  248,    2, 0x0a,   54 /* Public */,
      40,    1,  253,    2, 0x0a,   57 /* Public */,
      42,    0,  256,    2, 0x08,   59 /* Private */,
      43,    0,  257,    2, 0x08,   60 /* Private */,
      44,    0,  258,    2, 0x08,   61 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   15,   16,   17,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::QString,   20,   21,   22,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::Int,   20,   21,   17,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 25, 0x80000000 | 27,   15,   16,   26,   28,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Int, QMetaType::QString,   35,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,   38,   39,
    QMetaType::Void, QMetaType::UInt,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
      45, QMetaType::Bool, 0x00015001, uint(0), 0,
      46, QMetaType::Bool, 0x00015001, uint(1), 0,
      47, QMetaType::Bool, 0x00015001, uint(2), 0,
      14, QMetaType::Bool, 0x00015001, uint(3), 0,
      48, QMetaType::Int, 0x00015001, uint(4), 0,
       8, QMetaType::QString, 0x00015001, uint(5), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject Originull::Logind::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSOriginullSCOPELogindENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSOriginullSCOPELogindENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSOriginullSCOPELogindENDCLASS_t,
        // property 'connected'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'hasSessionControl'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'sessionActive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'inhibited'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'vtNumber'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'seat'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Logind, std::true_type>,
        // method 'connectedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'hasSessionControlChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'sessionActiveChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'inhibitedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'vtNumberChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'seatChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'prepareForSleep'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'prepareForShutdown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'lockSessionRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unlockSessionRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'inhibited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'uninhibited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'devicePaused'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'deviceResumed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'inhibit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<Logind::InhibitFlags, std::false_type>,
        QtPrivate::TypeAndForceComplete<Logind::InhibitMode, std::false_type>,
        // method 'uninhibit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'lockSession'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unlockSession'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'takeControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'releaseControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'takeDevice'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'releaseDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'pauseDeviceComplete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'switchTo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method '_q_serviceRegistered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method '_q_serviceUnregistered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method '_q_sessionPropertiesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Originull::Logind::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Logind *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectedChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->hasSessionControlChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->sessionActiveChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->inhibitedChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->vtNumberChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->seatChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->prepareForSleep((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->prepareForShutdown((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->lockSessionRequested(); break;
        case 9: _t->unlockSessionRequested(); break;
        case 10: _t->inhibited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 11: _t->uninhibited((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->devicePaused((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 13: _t->deviceResumed((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 14: _t->inhibit((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<Logind::InhibitFlags>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<Logind::InhibitMode>>(_a[4]))); break;
        case 15: _t->uninhibit((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->lockSession(); break;
        case 17: _t->unlockSession(); break;
        case 18: _t->takeControl(); break;
        case 19: _t->releaseControl(); break;
        case 20: { int _r = _t->takeDevice((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->releaseDevice((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 22: _t->pauseDeviceComplete((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 23: _t->switchTo((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1]))); break;
        case 24: _t->d_func()->_q_serviceRegistered(); break;
        case 25: _t->d_func()->_q_serviceUnregistered(); break;
        case 26: _t->d_func()->_q_sessionPropertiesChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Logind::*)(bool );
            if (_t _q_method = &Logind::connectedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Logind::*)(bool );
            if (_t _q_method = &Logind::hasSessionControlChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Logind::*)(bool );
            if (_t _q_method = &Logind::sessionActiveChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Logind::*)(bool );
            if (_t _q_method = &Logind::inhibitedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Logind::*)(int );
            if (_t _q_method = &Logind::vtNumberChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Logind::*)(const QString & );
            if (_t _q_method = &Logind::seatChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Logind::*)(bool );
            if (_t _q_method = &Logind::prepareForSleep; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Logind::*)(bool );
            if (_t _q_method = &Logind::prepareForShutdown; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Logind::*)();
            if (_t _q_method = &Logind::lockSessionRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Logind::*)();
            if (_t _q_method = &Logind::unlockSessionRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (Logind::*)(const QString & , const QString & , int );
            if (_t _q_method = &Logind::inhibited; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (Logind::*)(int );
            if (_t _q_method = &Logind::uninhibited; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (Logind::*)(quint32 , quint32 , const QString & );
            if (_t _q_method = &Logind::devicePaused; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (Logind::*)(quint32 , quint32 , int );
            if (_t _q_method = &Logind::deviceResumed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Logind *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->hasSessionControl(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isSessionActive(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->isInhibited(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->vtNumber(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->seat(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *Originull::Logind::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Originull::Logind::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSOriginullSCOPELogindENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Originull::Logind::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 27;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Originull::Logind::connectedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Originull::Logind::hasSessionControlChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Originull::Logind::sessionActiveChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Originull::Logind::inhibitedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Originull::Logind::vtNumberChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Originull::Logind::seatChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Originull::Logind::prepareForSleep(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Originull::Logind::prepareForShutdown(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Originull::Logind::lockSessionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void Originull::Logind::unlockSessionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void Originull::Logind::inhibited(const QString & _t1, const QString & _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Originull::Logind::uninhibited(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Originull::Logind::devicePaused(quint32 _t1, quint32 _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Originull::Logind::deviceResumed(quint32 _t1, quint32 _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}
QT_WARNING_POP
