/****************************************************************************
** Meta object code from reading C++ file 'filesystemmodel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/filesystemmodel.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filesystemmodel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSLSFSModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLSFSModelENDCLASS = QtMocHelpers::stringData(
    "LSFSModel",
    "rootPathChanged",
    "",
    "newPath",
    "fileRenamed",
    "path",
    "oldName",
    "newName",
    "directoryLoaded",
    "sortingChanged",
    "_q_directoryChanged",
    "directory",
    "list",
    "_q_performDelayedSort",
    "_q_fileSystemChanged",
    "QList<std::pair<QString,QFileInfo>>",
    "_q_resolvedName",
    "fileName",
    "resolvedName",
    "resolveSymlinks",
    "readOnly",
    "nameFilterDisables",
    "Option",
    "DontWatchForChanges",
    "DontResolveSymlinks",
    "DontUseCustomDirectoryIcons"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLSFSModelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       3,   92, // properties
       1,  107, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    5 /* Public */,
       4,    3,   65,    2, 0x06,    7 /* Public */,
       8,    1,   72,    2, 0x06,   11 /* Public */,
       9,    0,   75,    2, 0x06,   13 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    2,   76,    2, 0x08,   14 /* Private */,
      13,    0,   81,    2, 0x08,   17 /* Private */,
      14,    2,   82,    2, 0x08,   18 /* Private */,
      16,    2,   87,    2, 0x08,   21 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    5,    6,    7,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList,   11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 15,    5,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   17,   18,

 // properties: name, type, flags, notifyId, revision
      19, QMetaType::Bool, 0x00015103, uint(-1), 0,
      20, QMetaType::Bool, 0x00015103, uint(-1), 0,
      21, QMetaType::Bool, 0x00015103, uint(-1), 0,

 // enums: name, alias, flags, count, data
      22,   22, 0x0,    3,  112,

 // enum data: key, value
      23, uint(LSFSModel::DontWatchForChanges),
      24, uint(LSFSModel::DontResolveSymlinks),
      25, uint(LSFSModel::DontUseCustomDirectoryIcons),

       0        // eod
};

Q_CONSTINIT const QMetaObject LSFSModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractItemModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSLSFSModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLSFSModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLSFSModelENDCLASS_t,
        // property 'resolveSymlinks'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'readOnly'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'nameFilterDisables'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // enum 'Option'
        QtPrivate::TypeAndForceComplete<LSFSModel::Option, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LSFSModel, std::true_type>,
        // method 'rootPathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fileRenamed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'directoryLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'sortingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method '_q_directoryChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method '_q_performDelayedSort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method '_q_fileSystemChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<QPair<QString,QFileInfo> > &, std::false_type>,
        // method '_q_resolvedName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void LSFSModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LSFSModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->rootPathChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->fileRenamed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->directoryLoaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->sortingChanged(); break;
        case 4: _t->d_func()->_q_directoryChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2]))); break;
        case 5: _t->d_func()->_q_performDelayedSort(); break;
        case 6: _t->d_func()->_q_fileSystemChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<std::pair<QString,QFileInfo>>>>(_a[2]))); break;
        case 7: _t->d_func()->_q_resolvedName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LSFSModel::*)(const QString & );
            if (_t _q_method = &LSFSModel::rootPathChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LSFSModel::*)(const QString & , const QString & , const QString & );
            if (_t _q_method = &LSFSModel::fileRenamed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LSFSModel::*)(const QString & );
            if (_t _q_method = &LSFSModel::directoryLoaded; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LSFSModel::*)();
            if (_t _q_method = &LSFSModel::sortingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<LSFSModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->resolveSymlinks(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isReadOnly(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->nameFilterDisables(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<LSFSModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setResolveSymlinks(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setNameFilterDisables(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *LSFSModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LSFSModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLSFSModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractItemModel::qt_metacast(_clname);
}

int LSFSModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LSFSModel::rootPathChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LSFSModel::fileRenamed(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LSFSModel::directoryLoaded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LSFSModel::sortingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
