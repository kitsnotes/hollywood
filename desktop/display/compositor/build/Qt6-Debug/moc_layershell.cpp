/****************************************************************************
** Meta object code from reading C++ file 'layershell.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/layershell.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'layershell.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSWlrLayerShellV1ENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSWlrLayerShellV1ENDCLASS = QtMocHelpers::stringData(
    "WlrLayerShellV1",
    "layerSurfaceCreated",
    "",
    "WlrLayerSurfaceV1*",
    "layerSurface",
    "Layer",
    "BackgroundLayer",
    "BottomLayer",
    "TopLayer",
    "OverlayLayer"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWlrLayerShellV1ENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       1,   23, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    2 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // enums: name, alias, flags, count, data
       5,    5, 0x0,    4,   28,

 // enum data: key, value
       6, uint(WlrLayerShellV1::BackgroundLayer),
       7, uint(WlrLayerShellV1::BottomLayer),
       8, uint(WlrLayerShellV1::TopLayer),
       9, uint(WlrLayerShellV1::OverlayLayer),

       0        // eod
};

Q_CONSTINIT const QMetaObject WlrLayerShellV1::staticMetaObject = { {
    QMetaObject::SuperData::link<QWaylandCompositorExtensionTemplate<WlrLayerShellV1>::staticMetaObject>(),
    qt_meta_stringdata_CLASSWlrLayerShellV1ENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWlrLayerShellV1ENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWlrLayerShellV1ENDCLASS_t,
        // enum 'Layer'
        QtPrivate::TypeAndForceComplete<WlrLayerShellV1::Layer, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WlrLayerShellV1, std::true_type>,
        // method 'layerSurfaceCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<WlrLayerSurfaceV1 *, std::false_type>
    >,
    nullptr
} };

void WlrLayerShellV1::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WlrLayerShellV1 *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->layerSurfaceCreated((*reinterpret_cast< std::add_pointer_t<WlrLayerSurfaceV1*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< WlrLayerSurfaceV1* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WlrLayerShellV1::*)(WlrLayerSurfaceV1 * );
            if (_t _q_method = &WlrLayerShellV1::layerSurfaceCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *WlrLayerShellV1::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WlrLayerShellV1::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWlrLayerShellV1ENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWaylandServer::zwlr_layer_shell_v1"))
        return static_cast< QtWaylandServer::zwlr_layer_shell_v1*>(this);
    return QWaylandCompositorExtensionTemplate<WlrLayerShellV1>::qt_metacast(_clname);
}

int WlrLayerShellV1::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWaylandCompositorExtensionTemplate<WlrLayerShellV1>::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void WlrLayerShellV1::layerSurfaceCreated(WlrLayerSurfaceV1 * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSWlrLayerSurfaceV1ENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSWlrLayerSurfaceV1ENDCLASS = QtMocHelpers::stringData(
    "WlrLayerSurfaceV1",
    "layerChanged",
    "",
    "sizeChanged",
    "anchorsChanged",
    "exclusiveZoneChanged",
    "leftMarginChanged",
    "topMarginChanged",
    "rightMarginChanged",
    "bottomMarginChanged",
    "keyboardInteractivityChanged",
    "changed",
    "mappedChanged",
    "configuredChanged",
    "xdgPopupParentChanged",
    "HWWaylandXdgPopup*",
    "popup",
    "Anchor",
    "TopAnchor",
    "BottomAnchor",
    "LeftAnchor",
    "RightAnchor",
    "Anchors",
    "KeyboardInteractivity",
    "NoKeyboardInteractivity",
    "ExclusiveKeyboardInteractivity",
    "OnDemandKeyboardInteractivity"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWlrLayerSurfaceV1ENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       3,  107, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x06,    4 /* Public */,
       3,    0,   93,    2, 0x06,    5 /* Public */,
       4,    0,   94,    2, 0x06,    6 /* Public */,
       5,    0,   95,    2, 0x06,    7 /* Public */,
       6,    0,   96,    2, 0x06,    8 /* Public */,
       7,    0,   97,    2, 0x06,    9 /* Public */,
       8,    0,   98,    2, 0x06,   10 /* Public */,
       9,    0,   99,    2, 0x06,   11 /* Public */,
      10,    0,  100,    2, 0x06,   12 /* Public */,
      11,    0,  101,    2, 0x06,   13 /* Public */,
      12,    0,  102,    2, 0x06,   14 /* Public */,
      13,    0,  103,    2, 0x06,   15 /* Public */,
      14,    1,  104,    2, 0x06,   16 /* Public */,

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
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,

 // enums: name, alias, flags, count, data
      17,   17, 0x0,    4,  122,
      22,   17, 0x1,    4,  130,
      23,   23, 0x0,    3,  138,

 // enum data: key, value
      18, uint(WlrLayerSurfaceV1::TopAnchor),
      19, uint(WlrLayerSurfaceV1::BottomAnchor),
      20, uint(WlrLayerSurfaceV1::LeftAnchor),
      21, uint(WlrLayerSurfaceV1::RightAnchor),
      18, uint(WlrLayerSurfaceV1::TopAnchor),
      19, uint(WlrLayerSurfaceV1::BottomAnchor),
      20, uint(WlrLayerSurfaceV1::LeftAnchor),
      21, uint(WlrLayerSurfaceV1::RightAnchor),
      24, uint(WlrLayerSurfaceV1::NoKeyboardInteractivity),
      25, uint(WlrLayerSurfaceV1::ExclusiveKeyboardInteractivity),
      26, uint(WlrLayerSurfaceV1::OnDemandKeyboardInteractivity),

       0        // eod
};

Q_CONSTINIT const QMetaObject WlrLayerSurfaceV1::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSWlrLayerSurfaceV1ENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWlrLayerSurfaceV1ENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWlrLayerSurfaceV1ENDCLASS_t,
        // enum 'Anchor'
        QtPrivate::TypeAndForceComplete<WlrLayerSurfaceV1::Anchor, std::true_type>,
        // enum 'Anchors'
        QtPrivate::TypeAndForceComplete<WlrLayerSurfaceV1::Anchors, std::true_type>,
        // enum 'KeyboardInteractivity'
        QtPrivate::TypeAndForceComplete<WlrLayerSurfaceV1::KeyboardInteractivity, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WlrLayerSurfaceV1, std::true_type>,
        // method 'layerChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'anchorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exclusiveZoneChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'leftMarginChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'topMarginChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rightMarginChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'bottomMarginChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'keyboardInteractivityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mappedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'configuredChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'xdgPopupParentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<HWWaylandXdgPopup *, std::false_type>
    >,
    nullptr
} };

void WlrLayerSurfaceV1::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WlrLayerSurfaceV1 *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->layerChanged(); break;
        case 1: _t->sizeChanged(); break;
        case 2: _t->anchorsChanged(); break;
        case 3: _t->exclusiveZoneChanged(); break;
        case 4: _t->leftMarginChanged(); break;
        case 5: _t->topMarginChanged(); break;
        case 6: _t->rightMarginChanged(); break;
        case 7: _t->bottomMarginChanged(); break;
        case 8: _t->keyboardInteractivityChanged(); break;
        case 9: _t->changed(); break;
        case 10: _t->mappedChanged(); break;
        case 11: _t->configuredChanged(); break;
        case 12: _t->xdgPopupParentChanged((*reinterpret_cast< std::add_pointer_t<HWWaylandXdgPopup*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< HWWaylandXdgPopup* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::layerChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::sizeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::anchorsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::exclusiveZoneChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::leftMarginChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::topMarginChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::rightMarginChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::bottomMarginChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::keyboardInteractivityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::changed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::mappedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)();
            if (_t _q_method = &WlrLayerSurfaceV1::configuredChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (WlrLayerSurfaceV1::*)(HWWaylandXdgPopup * );
            if (_t _q_method = &WlrLayerSurfaceV1::xdgPopupParentChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
    }
}

const QMetaObject *WlrLayerSurfaceV1::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WlrLayerSurfaceV1::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWlrLayerSurfaceV1ENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QtWaylandServer::zwlr_layer_surface_v1"))
        return static_cast< QtWaylandServer::zwlr_layer_surface_v1*>(this);
    return QObject::qt_metacast(_clname);
}

int WlrLayerSurfaceV1::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void WlrLayerSurfaceV1::layerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void WlrLayerSurfaceV1::sizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void WlrLayerSurfaceV1::anchorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void WlrLayerSurfaceV1::exclusiveZoneChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void WlrLayerSurfaceV1::leftMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void WlrLayerSurfaceV1::topMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void WlrLayerSurfaceV1::rightMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void WlrLayerSurfaceV1::bottomMarginChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void WlrLayerSurfaceV1::keyboardInteractivityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void WlrLayerSurfaceV1::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void WlrLayerSurfaceV1::mappedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void WlrLayerSurfaceV1::configuredChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void WlrLayerSurfaceV1::xdgPopupParentChanged(HWWaylandXdgPopup * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP
