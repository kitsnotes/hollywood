/****************************************************************************
** Meta object code from reading C++ file 'locationbar.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/locationbar.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'locationbar.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSLSLocationBarENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLSLocationBarENDCLASS = QtMocHelpers::stringData(
    "LSLocationBar",
    "chdir",
    "",
    "path",
    "middleClickChdir",
    "editingFinished",
    "openEditor",
    "closeEditor",
    "copyPath",
    "onButtonToggled",
    "checked",
    "onScrollButtonClicked",
    "onReturnPressed",
    "setArrowEnabledState",
    "value",
    "setScrollButtonVisibility",
    "ensureToggledVisible"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLSLocationBarENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       4,    1,   89,    2, 0x06,    3 /* Public */,
       5,    0,   92,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   93,    2, 0x0a,    6 /* Public */,
       7,    0,   94,    2, 0x0a,    7 /* Public */,
       8,    0,   95,    2, 0x0a,    8 /* Public */,
       9,    1,   96,    2, 0x08,    9 /* Private */,
      11,    0,   99,    2, 0x08,   11 /* Private */,
      12,    0,  100,    2, 0x08,   12 /* Private */,
      13,    1,  101,    2, 0x08,   13 /* Private */,
      15,    0,  104,    2, 0x08,   15 /* Private */,
      16,    0,  105,    2, 0x08,   16 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LSLocationBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSLSLocationBarENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLSLocationBarENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLSLocationBarENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LSLocationBar, std::true_type>,
        // method 'chdir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'middleClickChdir'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openEditor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'closeEditor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copyPath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onButtonToggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onScrollButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReturnPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setArrowEnabledState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setScrollButtonVisibility'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ensureToggledVisible'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LSLocationBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LSLocationBar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->chdir((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 1: _t->middleClickChdir((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 2: _t->editingFinished(); break;
        case 3: _t->openEditor(); break;
        case 4: _t->closeEditor(); break;
        case 5: _t->copyPath(); break;
        case 6: _t->onButtonToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->onScrollButtonClicked(); break;
        case 8: _t->onReturnPressed(); break;
        case 9: _t->setArrowEnabledState((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->setScrollButtonVisibility(); break;
        case 11: _t->ensureToggledVisible(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LSLocationBar::*)(const QUrl & );
            if (_t _q_method = &LSLocationBar::chdir; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LSLocationBar::*)(const QUrl & );
            if (_t _q_method = &LSLocationBar::middleClickChdir; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LSLocationBar::*)();
            if (_t _q_method = &LSLocationBar::editingFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *LSLocationBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LSLocationBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLSLocationBarENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LSLocationBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void LSLocationBar::chdir(const QUrl & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LSLocationBar::middleClickChdir(const QUrl & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LSLocationBar::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
