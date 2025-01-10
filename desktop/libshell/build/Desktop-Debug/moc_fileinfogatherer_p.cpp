/****************************************************************************
** Meta object code from reading C++ file 'fileinfogatherer_p.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/private/fileinfogatherer_p.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fileinfogatherer_p.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSLSFileInfoGathererENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLSFileInfoGathererENDCLASS = QtMocHelpers::stringData(
    "LSFileInfoGatherer",
    "updates",
    "",
    "directory",
    "QList<std::pair<QString,QFileInfo>>",
    "newListOfFiles",
    "listOfFiles",
    "nameResolved",
    "fileName",
    "resolvedName",
    "directoryLoaded",
    "path",
    "list",
    "directoryPath",
    "fetchExtendedInformation",
    "files",
    "updateFile",
    "setResolveSymlinks",
    "enable",
    "setIconProvider",
    "HWFileIconProvider*",
    "provider"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLSFileInfoGathererENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   68,    2, 0x06,    1 /* Public */,
       5,    2,   73,    2, 0x106,    4 /* Public | MethodIsConst  */,
       7,    2,   78,    2, 0x106,    7 /* Public | MethodIsConst  */,
      10,    1,   83,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    1,   86,    2, 0x0a,   12 /* Public */,
      14,    2,   89,    2, 0x0a,   14 /* Public */,
      16,    1,   94,    2, 0x0a,   17 /* Public */,
      17,    1,   97,    2, 0x0a,   19 /* Public */,
      19,    1,  100,    2, 0x0a,   21 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    1,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList,    3,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList,   11,   15,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void, 0x80000000 | 20,   21,

       0        // eod
};

Q_CONSTINIT const QMetaObject LSFileInfoGatherer::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_CLASSLSFileInfoGathererENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLSFileInfoGathererENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLSFileInfoGathererENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LSFileInfoGatherer, std::true_type>,
        // method 'updates'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<QPair<QString,QFileInfo> > &, std::false_type>,
        // method 'newListOfFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'nameResolved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'directoryLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'list'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fetchExtendedInformation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'updateFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setResolveSymlinks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setIconProvider'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWFileIconProvider *, std::false_type>
    >,
    nullptr
} };

void LSFileInfoGatherer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LSFileInfoGatherer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updates((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<std::pair<QString,QFileInfo>>>>(_a[2]))); break;
        case 1: _t->newListOfFiles((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2]))); break;
        case 2: _t->nameResolved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->directoryLoaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->list((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->fetchExtendedInformation((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2]))); break;
        case 6: _t->updateFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->setResolveSymlinks((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->setIconProvider((*reinterpret_cast< std::add_pointer_t<HWFileIconProvider*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LSFileInfoGatherer::*)(const QString & , const QVector<QPair<QString,QFileInfo> > & );
            if (_t _q_method = &LSFileInfoGatherer::updates; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LSFileInfoGatherer::*)(const QString & , const QStringList & ) const;
            if (_t _q_method = &LSFileInfoGatherer::newListOfFiles; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LSFileInfoGatherer::*)(const QString & , const QString & ) const;
            if (_t _q_method = &LSFileInfoGatherer::nameResolved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LSFileInfoGatherer::*)(const QString & );
            if (_t _q_method = &LSFileInfoGatherer::directoryLoaded; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *LSFileInfoGatherer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LSFileInfoGatherer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLSFileInfoGathererENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int LSFileInfoGatherer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void LSFileInfoGatherer::updates(const QString & _t1, const QVector<QPair<QString,QFileInfo> > & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LSFileInfoGatherer::newListOfFiles(const QString & _t1, const QStringList & _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< LSFileInfoGatherer *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LSFileInfoGatherer::nameResolved(const QString & _t1, const QString & _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< LSFileInfoGatherer *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LSFileInfoGatherer::directoryLoaded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
