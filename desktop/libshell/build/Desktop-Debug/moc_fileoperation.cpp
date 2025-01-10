/****************************************************************************
** Meta object code from reading C++ file 'fileoperation.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/private/fileoperation.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fileoperation.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSFileOperationENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSFileOperationENDCLASS = QtMocHelpers::stringData(
    "FileOperation",
    "error",
    "",
    "id",
    "FileOperation::Error",
    "stopped",
    "stateChanged",
    "FileOperation::State",
    "state",
    "done",
    "started",
    "dataTransferProgress",
    "progress",
    "finished",
    "canceled",
    "cancelAll",
    "cancel",
    "skip",
    "skipAll",
    "retry",
    "overwrite",
    "overwriteAll",
    "reset",
    "resetSkip",
    "resetOverwrite",
    "copyStarted",
    "copyFinished",
    "copyCanceled",
    "copyError",
    "progressRequest",
    "progressInterval",
    "autoReset",
    "CopyFlag",
    "NonInteractive",
    "Force",
    "MakeLinks",
    "FollowLinks"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSFileOperationENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       2,  202, // properties
       1,  212, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,  146,    2, 0x06,    4 /* Public */,
       6,    1,  153,    2, 0x06,    8 /* Public */,
       9,    1,  156,    2, 0x06,   10 /* Public */,
      10,    1,  159,    2, 0x06,   12 /* Public */,
      11,    2,  162,    2, 0x06,   14 /* Public */,
      13,    2,  167,    2, 0x06,   17 /* Public */,
      14,    0,  172,    2, 0x06,   20 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    0,  173,    2, 0x0a,   21 /* Public */,
      16,    1,  174,    2, 0x0a,   22 /* Public */,
      17,    0,  177,    2, 0x0a,   24 /* Public */,
      18,    0,  178,    2, 0x0a,   25 /* Public */,
      19,    0,  179,    2, 0x0a,   26 /* Public */,
      20,    0,  180,    2, 0x0a,   27 /* Public */,
      21,    0,  181,    2, 0x0a,   28 /* Public */,
      22,    0,  182,    2, 0x0a,   29 /* Public */,
      23,    0,  183,    2, 0x0a,   30 /* Public */,
      24,    0,  184,    2, 0x0a,   31 /* Public */,
      25,    1,  185,    2, 0x08,   32 /* Private */,
      26,    2,  188,    2, 0x08,   34 /* Private */,
      27,    0,  193,    2, 0x08,   37 /* Private */,
      28,    3,  194,    2, 0x08,   38 /* Private */,
      29,    0,  201,    2, 0x08,   42 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4, QMetaType::Bool,    3,    1,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Bool,    1,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong,    3,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    1,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4, QMetaType::Bool,    2,    2,    2,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
      30, QMetaType::Int, 0x00015103, uint(-1), 0,
      31, QMetaType::Bool, 0x00015103, uint(-1), 0,

 // enums: name, alias, flags, count, data
      32,   32, 0x1,    4,  217,

 // enum data: key, value
      33, uint(FileOperation::NonInteractive),
      34, uint(FileOperation::Force),
      35, uint(FileOperation::MakeLinks),
      36, uint(FileOperation::FollowLinks),

       0        // eod
};

Q_CONSTINIT const QMetaObject FileOperation::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSFileOperationENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSFileOperationENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSFileOperationENDCLASS_t,
        // property 'progressInterval'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'autoReset'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // enum 'CopyFlag'
        QtPrivate::TypeAndForceComplete<FileOperation::CopyFlag, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FileOperation, std::true_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<FileOperation::Error, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<FileOperation::State, std::false_type>,
        // method 'done'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'started'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'dataTransferProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'canceled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cancelAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'skip'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'skipAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'retry'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'overwrite'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'overwriteAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetSkip'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetOverwrite'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copyStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'copyFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'copyCanceled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copyError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<FileOperation::Error, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'progressRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void FileOperation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperation *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->error((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<FileOperation::Error>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 1: _t->stateChanged((*reinterpret_cast< std::add_pointer_t<FileOperation::State>>(_a[1]))); break;
        case 2: _t->done((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->started((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->dataTransferProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 5: _t->finished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->canceled(); break;
        case 7: _t->cancelAll(); break;
        case 8: _t->cancel((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->skip(); break;
        case 10: _t->skipAll(); break;
        case 11: _t->retry(); break;
        case 12: _t->overwrite(); break;
        case 13: _t->overwriteAll(); break;
        case 14: _t->reset(); break;
        case 15: _t->resetSkip(); break;
        case 16: _t->resetOverwrite(); break;
        case 17: _t->d_func()->copyStarted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->d_func()->copyFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 19: _t->d_func()->copyCanceled(); break;
        case 20: _t->d_func()->copyError((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<FileOperation::Error>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 21: _t->d_func()->progressRequest(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperation::*)(int , FileOperation::Error , bool );
            if (_t _q_method = &FileOperation::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(FileOperation::State );
            if (_t _q_method = &FileOperation::stateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(bool );
            if (_t _q_method = &FileOperation::done; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(int );
            if (_t _q_method = &FileOperation::started; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(int , qint64 );
            if (_t _q_method = &FileOperation::dataTransferProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(int , bool );
            if (_t _q_method = &FileOperation::finished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (_t _q_method = &FileOperation::canceled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<FileOperation *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->progressInterval(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->autoReset(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<FileOperation *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setProgressInterval(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setAutoReset(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *FileOperation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileOperation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSFileOperationENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FileOperation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void FileOperation::error(int _t1, FileOperation::Error _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FileOperation::stateChanged(FileOperation::State _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FileOperation::done(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FileOperation::started(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void FileOperation::dataTransferProgress(int _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void FileOperation::finished(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void FileOperation::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSOperationThreadENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSOperationThreadENDCLASS = QtMocHelpers::stringData(
    "OperationThread",
    "error",
    "",
    "id",
    "FileOperation::Error",
    "stopped",
    "started",
    "dataTransferProgress",
    "progress",
    "finished",
    "canceled",
    "restart",
    "copy",
    "CopyRequest",
    "request",
    "QMap<int,CopyRequest>",
    "requests",
    "cancel",
    "skip",
    "skipAll",
    "retry",
    "overwrite",
    "overwriteAll",
    "resetOverwrite",
    "resetSkip",
    "copierDestroyed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSOperationThreadENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,  128,    2, 0x06,    1 /* Public */,
       6,    1,  135,    2, 0x06,    5 /* Public */,
       7,    2,  138,    2, 0x06,    7 /* Public */,
       9,    2,  143,    2, 0x06,   10 /* Public */,
      10,    0,  148,    2, 0x06,   13 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,  149,    2, 0x0a,   14 /* Public */,
      12,    2,  150,    2, 0x0a,   15 /* Public */,
      12,    1,  155,    2, 0x0a,   18 /* Public */,
      17,    0,  158,    2, 0x0a,   20 /* Public */,
      17,    1,  159,    2, 0x0a,   21 /* Public */,
      18,    0,  162,    2, 0x0a,   23 /* Public */,
      19,    0,  163,    2, 0x0a,   24 /* Public */,
      20,    0,  164,    2, 0x0a,   25 /* Public */,
      21,    0,  165,    2, 0x0a,   26 /* Public */,
      22,    0,  166,    2, 0x0a,   27 /* Public */,
      23,    0,  167,    2, 0x0a,   28 /* Public */,
      24,    0,  168,    2, 0x0a,   29 /* Public */,
       8,    0,  169,    2, 0x0a,   30 /* Public */,
      25,    0,  170,    2, 0x09,   31 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4, QMetaType::Bool,    3,    1,    5,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong,    3,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    1,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,    3,   14,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject OperationThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_CLASSOperationThreadENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSOperationThreadENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSOperationThreadENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OperationThread, std::true_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<FileOperation::Error, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'started'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'dataTransferProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'canceled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copy'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const CopyRequest &, std::false_type>,
        // method 'copy'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QMap<int,CopyRequest> &, std::false_type>,
        // method 'cancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'skip'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'skipAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'retry'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'overwrite'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'overwriteAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetOverwrite'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetSkip'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'progress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copierDestroyed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void OperationThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OperationThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->error((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<FileOperation::Error>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 1: _t->started((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->dataTransferProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 3: _t->finished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 4: _t->canceled(); break;
        case 5: _t->restart(); break;
        case 6: _t->copy((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CopyRequest>>(_a[2]))); break;
        case 7: _t->copy((*reinterpret_cast< std::add_pointer_t<QMap<int,CopyRequest>>>(_a[1]))); break;
        case 8: _t->cancel(); break;
        case 9: _t->cancel((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->skip(); break;
        case 11: _t->skipAll(); break;
        case 12: _t->retry(); break;
        case 13: _t->overwrite(); break;
        case 14: _t->overwriteAll(); break;
        case 15: _t->resetOverwrite(); break;
        case 16: _t->resetSkip(); break;
        case 17: _t->progress(); break;
        case 18: _t->copierDestroyed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OperationThread::*)(int , FileOperation::Error , bool );
            if (_t _q_method = &OperationThread::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OperationThread::*)(int );
            if (_t _q_method = &OperationThread::started; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (OperationThread::*)(int , qint64 );
            if (_t _q_method = &OperationThread::dataTransferProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (OperationThread::*)(int , bool );
            if (_t _q_method = &OperationThread::finished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (OperationThread::*)();
            if (_t _q_method = &OperationThread::canceled; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *OperationThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OperationThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSOperationThreadENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int OperationThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void OperationThread::error(int _t1, FileOperation::Error _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OperationThread::started(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OperationThread::dataTransferProgress(int _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void OperationThread::finished(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void OperationThread::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
