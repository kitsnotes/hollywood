/****************************************************************************
** Meta object code from reading C++ file 'wndmgmt.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/wndmgmt.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wndmgmt.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSPlasmaWindowManagementENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSPlasmaWindowManagementENDCLASS = QtMocHelpers::stringData(
    "PlasmaWindowManagement"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPlasmaWindowManagementENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject PlasmaWindowManagement::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandCompositorExtensionTemplate<PlasmaWindowManagement>::staticMetaObject>(),
    qt_meta_stringdata_CLASSPlasmaWindowManagementENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPlasmaWindowManagementENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPlasmaWindowManagementENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PlasmaWindowManagement, std::true_type>
    >,
    nullptr
} };

void PlasmaWindowManagement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PlasmaWindowManagement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlasmaWindowManagement::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPlasmaWindowManagementENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWaylandServer::org_kde_plasma_window_management"))
        return static_cast< QtWaylandServer::org_kde_plasma_window_management*>(this);
    return QWaylandCompositorExtensionTemplate<PlasmaWindowManagement>::qt_metacast(_clname);
}

int PlasmaWindowManagement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandCompositorExtensionTemplate<PlasmaWindowManagement>::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSPlasmaWindowControlENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSPlasmaWindowControlENDCLASS = QtMocHelpers::stringData(
    "PlasmaWindowControl",
    "setStateRequest",
    "",
    "flags",
    "state",
    "getIconRequest",
    "fd",
    "titleChanged",
    "title",
    "themedIconChanged",
    "icon",
    "closeRequested",
    "moveRequested",
    "resizeRequested",
    "activeRequested",
    "set",
    "minimizedRequested",
    "maximizedRequested",
    "fullscreenRequested",
    "destroy",
    "setActive",
    "setMinimized",
    "setMaximized",
    "setMinimizable",
    "setMaximizable",
    "setCloseable",
    "setTitle",
    "setIcon",
    "setThemedIcon"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPlasmaWindowControlENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,  140,    2, 0x06,    1 /* Public */,
       5,    1,  145,    2, 0x06,    4 /* Public */,
       7,    1,  148,    2, 0x06,    6 /* Public */,
       9,    1,  151,    2, 0x06,    8 /* Public */,
      11,    0,  154,    2, 0x06,   10 /* Public */,
      12,    0,  155,    2, 0x06,   11 /* Public */,
      13,    0,  156,    2, 0x06,   12 /* Public */,
      14,    1,  157,    2, 0x06,   13 /* Public */,
      16,    1,  160,    2, 0x06,   15 /* Public */,
      17,    1,  163,    2, 0x06,   17 /* Public */,
      18,    1,  166,    2, 0x06,   19 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      19,    0,  169,    2, 0x0a,   21 /* Public */,
      20,    1,  170,    2, 0x0a,   22 /* Public */,
      21,    1,  173,    2, 0x0a,   24 /* Public */,
      22,    1,  176,    2, 0x0a,   26 /* Public */,
      23,    1,  179,    2, 0x0a,   28 /* Public */,
      24,    1,  182,    2, 0x0a,   30 /* Public */,
      25,    1,  185,    2, 0x0a,   32 /* Public */,
      26,    1,  188,    2, 0x0a,   34 /* Public */,
      27,    1,  191,    2, 0x0a,   36 /* Public */,
      28,    1,  194,    2, 0x0a,   38 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QIcon,   10,
    QMetaType::Void, QMetaType::QString,    8,

       0        // eod
};

Q_CONSTINIT const QMetaObject PlasmaWindowControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandCompositorExtensionTemplate<PlasmaWindowControl>::staticMetaObject>(),
    qt_meta_stringdata_CLASSPlasmaWindowControlENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPlasmaWindowControlENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPlasmaWindowControlENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PlasmaWindowControl, std::true_type>,
        // method 'setStateRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'getIconRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'titleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'themedIconChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'closeRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'moveRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resizeRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'activeRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'minimizedRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'maximizedRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'fullscreenRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'destroy'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setActive'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setMinimized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setMaximized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setMinimizable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setMaximizable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setCloseable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setTitle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setIcon'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QIcon &, std::false_type>,
        // method 'setThemedIcon'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void PlasmaWindowControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlasmaWindowControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setStateRequest((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[2]))); break;
        case 1: _t->getIconRequest((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->titleChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->themedIconChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->closeRequested(); break;
        case 5: _t->moveRequested(); break;
        case 6: _t->resizeRequested(); break;
        case 7: _t->activeRequested((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->minimizedRequested((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->maximizedRequested((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->fullscreenRequested((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->destroy(); break;
        case 12: _t->setActive((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->setMinimized((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->setMaximized((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->setMinimizable((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->setMaximizable((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 17: _t->setCloseable((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 18: _t->setTitle((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->setIcon((*reinterpret_cast< std::add_pointer_t<QIcon>>(_a[1]))); break;
        case 20: _t->setThemedIcon((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlasmaWindowControl::*)(uint , uint );
            if (_t _q_method = &PlasmaWindowControl::setStateRequest; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(int );
            if (_t _q_method = &PlasmaWindowControl::getIconRequest; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(const QString & );
            if (_t _q_method = &PlasmaWindowControl::titleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(const QString & );
            if (_t _q_method = &PlasmaWindowControl::themedIconChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)();
            if (_t _q_method = &PlasmaWindowControl::closeRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)();
            if (_t _q_method = &PlasmaWindowControl::moveRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)();
            if (_t _q_method = &PlasmaWindowControl::resizeRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(bool );
            if (_t _q_method = &PlasmaWindowControl::activeRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(bool );
            if (_t _q_method = &PlasmaWindowControl::minimizedRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(bool );
            if (_t _q_method = &PlasmaWindowControl::maximizedRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (PlasmaWindowControl::*)(bool );
            if (_t _q_method = &PlasmaWindowControl::fullscreenRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
    }
}

const QMetaObject *PlasmaWindowControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlasmaWindowControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPlasmaWindowControlENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWaylandServer::org_kde_plasma_window"))
        return static_cast< QtWaylandServer::org_kde_plasma_window*>(this);
    return QWaylandCompositorExtensionTemplate<PlasmaWindowControl>::qt_metacast(_clname);
}

int PlasmaWindowControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandCompositorExtensionTemplate<PlasmaWindowControl>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void PlasmaWindowControl::setStateRequest(uint _t1, uint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlasmaWindowControl::getIconRequest(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PlasmaWindowControl::titleChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PlasmaWindowControl::themedIconChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PlasmaWindowControl::closeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PlasmaWindowControl::moveRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void PlasmaWindowControl::resizeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void PlasmaWindowControl::activeRequested(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void PlasmaWindowControl::minimizedRequested(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void PlasmaWindowControl::maximizedRequested(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void PlasmaWindowControl::fullscreenRequested(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_WARNING_POP
