/****************************************************************************
** Meta object code from reading C++ file 'shortcuts.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/shortcuts.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shortcuts.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSShortcutManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSShortcutManagerENDCLASS = QtMocHelpers::stringData(
    "ShortcutManager",
    "monitorBrightnessUp",
    "",
    "monitorBrightnessDown",
    "keyboardBrightnessUp",
    "keyboardBrightnessDown",
    "printScreenRequested",
    "windowSwitchRequested",
    "volumeDownRequested",
    "volumeUpRequested",
    "volumeMuteRequested",
    "webBrowserRequested",
    "searchRequested",
    "reloadConfig"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSShortcutManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    1 /* Public */,
       3,    0,   87,    2, 0x06,    2 /* Public */,
       4,    0,   88,    2, 0x06,    3 /* Public */,
       5,    0,   89,    2, 0x06,    4 /* Public */,
       6,    0,   90,    2, 0x06,    5 /* Public */,
       7,    0,   91,    2, 0x06,    6 /* Public */,
       8,    0,   92,    2, 0x06,    7 /* Public */,
       9,    0,   93,    2, 0x06,    8 /* Public */,
      10,    0,   94,    2, 0x06,    9 /* Public */,
      11,    0,   95,    2, 0x06,   10 /* Public */,
      12,    0,   96,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    0,   97,    2, 0x0a,   12 /* Public */,

 // signals: parameters
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
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ShortcutManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSShortcutManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSShortcutManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSShortcutManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ShortcutManager, std::true_type>,
        // method 'monitorBrightnessUp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'monitorBrightnessDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'keyboardBrightnessUp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'keyboardBrightnessDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'printScreenRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'windowSwitchRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'volumeDownRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'volumeUpRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'volumeMuteRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'webBrowserRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'searchRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reloadConfig'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ShortcutManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ShortcutManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->monitorBrightnessUp(); break;
        case 1: _t->monitorBrightnessDown(); break;
        case 2: _t->keyboardBrightnessUp(); break;
        case 3: _t->keyboardBrightnessDown(); break;
        case 4: _t->printScreenRequested(); break;
        case 5: _t->windowSwitchRequested(); break;
        case 6: _t->volumeDownRequested(); break;
        case 7: _t->volumeUpRequested(); break;
        case 8: _t->volumeMuteRequested(); break;
        case 9: _t->webBrowserRequested(); break;
        case 10: _t->searchRequested(); break;
        case 11: _t->reloadConfig(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::monitorBrightnessUp; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::monitorBrightnessDown; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::keyboardBrightnessUp; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::keyboardBrightnessDown; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::printScreenRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::windowSwitchRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::volumeDownRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::volumeUpRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::volumeMuteRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::webBrowserRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ShortcutManager::*)();
            if (_t _q_method = &ShortcutManager::searchRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *ShortcutManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShortcutManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSShortcutManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ShortcutManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ShortcutManager::monitorBrightnessUp()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ShortcutManager::monitorBrightnessDown()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ShortcutManager::keyboardBrightnessUp()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ShortcutManager::keyboardBrightnessDown()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ShortcutManager::printScreenRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ShortcutManager::windowSwitchRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ShortcutManager::volumeDownRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ShortcutManager::volumeUpRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ShortcutManager::volumeMuteRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ShortcutManager::webBrowserRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ShortcutManager::searchRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}
QT_WARNING_POP
