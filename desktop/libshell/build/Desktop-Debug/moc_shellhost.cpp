/****************************************************************************
** Meta object code from reading C++ file 'shellhost.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/shellhost.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shellhost.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSLSEmbeddedShellHostENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLSEmbeddedShellHostENDCLASS = QtMocHelpers::stringData(
    "LSEmbeddedShellHost",
    "updateWindowTitle",
    "",
    "title",
    "updateWindowIcon",
    "icon",
    "updateStatusBar",
    "status",
    "viewModeChanged",
    "sortOrderChanged",
    "navigateToPath",
    "path",
    "navigateToUrl",
    "newTabWithPath",
    "setIconListView",
    "updateSettings",
    "setTableListView",
    "setColumnView",
    "goBack",
    "goForward",
    "goEnclosingFolder",
    "toggleViewOptions",
    "doSort",
    "column",
    "Qt::SortOrder",
    "order",
    "getInformationRequested",
    "locationBarEditingFinished",
    "viewOptionsFinished",
    "currentTabChanged",
    "index",
    "tabCloseRequested",
    "viewClicked",
    "QModelIndex",
    "idx",
    "viewActivated",
    "viewSelectionChanged",
    "QItemSelection",
    "selected",
    "deselected",
    "placeClicked",
    "deviceActivated",
    "updateColumnWidget",
    "modelRootPathChanged",
    "createNewTab",
    "viewContextMenuRequested",
    "pos",
    "filesystemSortingChanged",
    "copyItems",
    "enablePaste",
    "enable",
    "count",
    "paste",
    "canUndoChanged",
    "canRedoChanged",
    "selectAll",
    "invertSelection",
    "newFolder",
    "openSelection",
    "rename",
    "trash",
    "archiveSelection"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLSEmbeddedShellHostENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      47,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  296,    2, 0x06,    1 /* Public */,
       4,    1,  299,    2, 0x06,    3 /* Public */,
       6,    1,  302,    2, 0x06,    5 /* Public */,
       8,    0,  305,    2, 0x06,    7 /* Public */,
       9,    0,  306,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    1,  307,    2, 0x0a,    9 /* Public */,
      12,    1,  310,    2, 0x0a,   11 /* Public */,
      13,    1,  313,    2, 0x0a,   13 /* Public */,
      14,    1,  316,    2, 0x0a,   15 /* Public */,
      14,    0,  319,    2, 0x2a,   17 /* Public | MethodCloned */,
      16,    1,  320,    2, 0x0a,   18 /* Public */,
      16,    0,  323,    2, 0x2a,   20 /* Public | MethodCloned */,
      17,    1,  324,    2, 0x0a,   21 /* Public */,
      17,    0,  327,    2, 0x2a,   23 /* Public | MethodCloned */,
      18,    0,  328,    2, 0x0a,   24 /* Public */,
      19,    0,  329,    2, 0x0a,   25 /* Public */,
      20,    0,  330,    2, 0x0a,   26 /* Public */,
      21,    1,  331,    2, 0x0a,   27 /* Public */,
      22,    2,  334,    2, 0x0a,   29 /* Public */,
      26,    0,  339,    2, 0x08,   32 /* Private */,
      27,    0,  340,    2, 0x08,   33 /* Private */,
      28,    1,  341,    2, 0x08,   34 /* Private */,
      29,    1,  344,    2, 0x08,   36 /* Private */,
      31,    1,  347,    2, 0x08,   38 /* Private */,
      32,    1,  350,    2, 0x08,   40 /* Private */,
      35,    1,  353,    2, 0x08,   42 /* Private */,
      36,    2,  356,    2, 0x08,   44 /* Private */,
      40,    1,  361,    2, 0x08,   47 /* Private */,
      41,    1,  364,    2, 0x08,   49 /* Private */,
      42,    1,  367,    2, 0x08,   51 /* Private */,
      43,    1,  370,    2, 0x08,   53 /* Private */,
      44,    0,  373,    2, 0x08,   55 /* Private */,
      45,    1,  374,    2, 0x08,   56 /* Private */,
      47,    0,  377,    2, 0x08,   58 /* Private */,
      48,    0,  378,    2, 0x08,   59 /* Private */,
      49,    2,  379,    2, 0x08,   60 /* Private */,
      49,    1,  384,    2, 0x28,   63 /* Private | MethodCloned */,
      52,    0,  387,    2, 0x08,   65 /* Private */,
      53,    0,  388,    2, 0x08,   66 /* Private */,
      54,    0,  389,    2, 0x08,   67 /* Private */,
      55,    0,  390,    2, 0x08,   68 /* Private */,
      56,    0,  391,    2, 0x08,   69 /* Private */,
      57,    0,  392,    2, 0x08,   70 /* Private */,
      58,    0,  393,    2, 0x08,   71 /* Private */,
      59,    0,  394,    2, 0x08,   72 /* Private */,
      60,    0,  395,    2, 0x08,   73 /* Private */,
      61,    0,  396,    2, 0x08,   74 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QIcon,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,   11,
    QMetaType::Bool, QMetaType::QUrl,   11,
    QMetaType::Void, QMetaType::QUrl,   11,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 24,   23,   25,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, 0x80000000 | 37, 0x80000000 | 37,   38,   39,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   46,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::UInt,   50,   51,
    QMetaType::Void, QMetaType::Bool,   50,
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

       0        // eod
};

Q_CONSTINIT const QMetaObject LSEmbeddedShellHost::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSLSEmbeddedShellHostENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLSEmbeddedShellHostENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLSEmbeddedShellHostENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LSEmbeddedShellHost, std::true_type>,
        // method 'updateWindowTitle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'updateWindowIcon'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QIcon &, std::false_type>,
        // method 'updateStatusBar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'viewModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sortOrderChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'navigateToPath'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'navigateToUrl'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'newTabWithPath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'setIconListView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setIconListView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setTableListView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setTableListView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setColumnView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setColumnView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goBack'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goForward'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goEnclosingFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleViewOptions'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'doSort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::SortOrder, std::false_type>,
        // method 'getInformationRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'locationBarEditingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'viewOptionsFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'currentTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'tabCloseRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'viewClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'viewActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'viewSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QItemSelection &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QItemSelection &, std::false_type>,
        // method 'placeClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'deviceActivated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'updateColumnWidget'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'modelRootPathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'createNewTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'viewContextMenuRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'filesystemSortingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'copyItems'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enablePaste'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'enablePaste'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'paste'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'canUndoChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'canRedoChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'invertSelection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'newFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openSelection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rename'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'trash'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'archiveSelection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LSEmbeddedShellHost::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LSEmbeddedShellHost *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWindowTitle((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->updateWindowIcon((*reinterpret_cast< std::add_pointer_t<QIcon>>(_a[1]))); break;
        case 2: _t->updateStatusBar((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->viewModeChanged(); break;
        case 4: _t->sortOrderChanged(); break;
        case 5: { bool _r = _t->navigateToPath((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->navigateToUrl((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->newTabWithPath((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 8: _t->setIconListView((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->setIconListView(); break;
        case 10: _t->setTableListView((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->setTableListView(); break;
        case 12: _t->setColumnView((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->setColumnView(); break;
        case 14: _t->goBack(); break;
        case 15: _t->goForward(); break;
        case 16: _t->goEnclosingFolder(); break;
        case 17: _t->toggleViewOptions((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 18: _t->doSort((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Qt::SortOrder>>(_a[2]))); break;
        case 19: _t->getInformationRequested(); break;
        case 20: _t->locationBarEditingFinished(); break;
        case 21: _t->viewOptionsFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 22: _t->currentTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->tabCloseRequested((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->viewClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 25: _t->viewActivated((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 26: _t->viewSelectionChanged((*reinterpret_cast< std::add_pointer_t<QItemSelection>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QItemSelection>>(_a[2]))); break;
        case 27: _t->placeClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 28: _t->deviceActivated((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 29: _t->updateColumnWidget((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 30: _t->modelRootPathChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 31: _t->createNewTab(); break;
        case 32: _t->viewContextMenuRequested((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 33: _t->filesystemSortingChanged(); break;
        case 34: _t->copyItems(); break;
        case 35: _t->enablePaste((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[2]))); break;
        case 36: _t->enablePaste((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 37: _t->paste(); break;
        case 38: _t->canUndoChanged(); break;
        case 39: _t->canRedoChanged(); break;
        case 40: _t->selectAll(); break;
        case 41: _t->invertSelection(); break;
        case 42: _t->newFolder(); break;
        case 43: _t->openSelection(); break;
        case 44: _t->rename(); break;
        case 45: _t->trash(); break;
        case 46: _t->archiveSelection(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 26:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QItemSelection >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LSEmbeddedShellHost::*)(const QString & );
            if (_t _q_method = &LSEmbeddedShellHost::updateWindowTitle; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LSEmbeddedShellHost::*)(const QIcon & );
            if (_t _q_method = &LSEmbeddedShellHost::updateWindowIcon; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LSEmbeddedShellHost::*)(const QString & );
            if (_t _q_method = &LSEmbeddedShellHost::updateStatusBar; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LSEmbeddedShellHost::*)();
            if (_t _q_method = &LSEmbeddedShellHost::viewModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (LSEmbeddedShellHost::*)();
            if (_t _q_method = &LSEmbeddedShellHost::sortOrderChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *LSEmbeddedShellHost::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LSEmbeddedShellHost::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLSEmbeddedShellHostENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LSEmbeddedShellHost::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    return _id;
}

// SIGNAL 0
void LSEmbeddedShellHost::updateWindowTitle(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LSEmbeddedShellHost::updateWindowIcon(const QIcon & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LSEmbeddedShellHost::updateStatusBar(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LSEmbeddedShellHost::viewModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void LSEmbeddedShellHost::sortOrderChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
