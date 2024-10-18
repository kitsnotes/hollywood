/****************************************************************************
** Meta object code from reading C++ file 'libinputhandler.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../libinput/hollywood/libinputhandler.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'libinputhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.2. It"
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
struct qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS = QtMocHelpers::stringData(
    "Originull::Platform::LibInputHandler",
    "ready",
    "",
    "capabilitiesChanged",
    "suspendedChanged",
    "keyboardCountChanged",
    "count",
    "pointerCountChanged",
    "touchCountChanged",
    "tabletCountChanged",
    "gestureCountChanged",
    "touchDeviceRegistered",
    "QPointingDevice*",
    "td",
    "touchDeviceUnregistered",
    "keyPressed",
    "Originull::Platform::LibInputKeyEvent",
    "event",
    "keyReleased",
    "mousePressed",
    "Originull::Platform::LibInputMouseEvent",
    "mouseReleased",
    "mouseMoved",
    "mouseWheel",
    "touchEvent",
    "Originull::Platform::LibInputTouchEvent",
    "touchCancel",
    "suspend",
    "resume",
    "handleEvents",
    "suspended"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS_t {
    uint offsetsAndSizes[62];
    char stringdata0[37];
    char stringdata1[6];
    char stringdata2[1];
    char stringdata3[20];
    char stringdata4[17];
    char stringdata5[21];
    char stringdata6[6];
    char stringdata7[20];
    char stringdata8[18];
    char stringdata9[19];
    char stringdata10[20];
    char stringdata11[22];
    char stringdata12[17];
    char stringdata13[3];
    char stringdata14[24];
    char stringdata15[11];
    char stringdata16[38];
    char stringdata17[6];
    char stringdata18[12];
    char stringdata19[13];
    char stringdata20[40];
    char stringdata21[14];
    char stringdata22[11];
    char stringdata23[11];
    char stringdata24[11];
    char stringdata25[40];
    char stringdata26[12];
    char stringdata27[8];
    char stringdata28[7];
    char stringdata29[13];
    char stringdata30[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS_t qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 36),  // "Originull::Platform::LibInput..."
        QT_MOC_LITERAL(37, 5),  // "ready"
        QT_MOC_LITERAL(43, 0),  // ""
        QT_MOC_LITERAL(44, 19),  // "capabilitiesChanged"
        QT_MOC_LITERAL(64, 16),  // "suspendedChanged"
        QT_MOC_LITERAL(81, 20),  // "keyboardCountChanged"
        QT_MOC_LITERAL(102, 5),  // "count"
        QT_MOC_LITERAL(108, 19),  // "pointerCountChanged"
        QT_MOC_LITERAL(128, 17),  // "touchCountChanged"
        QT_MOC_LITERAL(146, 18),  // "tabletCountChanged"
        QT_MOC_LITERAL(165, 19),  // "gestureCountChanged"
        QT_MOC_LITERAL(185, 21),  // "touchDeviceRegistered"
        QT_MOC_LITERAL(207, 16),  // "QPointingDevice*"
        QT_MOC_LITERAL(224, 2),  // "td"
        QT_MOC_LITERAL(227, 23),  // "touchDeviceUnregistered"
        QT_MOC_LITERAL(251, 10),  // "keyPressed"
        QT_MOC_LITERAL(262, 37),  // "Originull::Platform::LibInput..."
        QT_MOC_LITERAL(300, 5),  // "event"
        QT_MOC_LITERAL(306, 11),  // "keyReleased"
        QT_MOC_LITERAL(318, 12),  // "mousePressed"
        QT_MOC_LITERAL(331, 39),  // "Originull::Platform::LibInput..."
        QT_MOC_LITERAL(371, 13),  // "mouseReleased"
        QT_MOC_LITERAL(385, 10),  // "mouseMoved"
        QT_MOC_LITERAL(396, 10),  // "mouseWheel"
        QT_MOC_LITERAL(407, 10),  // "touchEvent"
        QT_MOC_LITERAL(418, 39),  // "Originull::Platform::LibInput..."
        QT_MOC_LITERAL(458, 11),  // "touchCancel"
        QT_MOC_LITERAL(470, 7),  // "suspend"
        QT_MOC_LITERAL(478, 6),  // "resume"
        QT_MOC_LITERAL(485, 12),  // "handleEvents"
        QT_MOC_LITERAL(498, 9)   // "suspended"
    },
    "Originull::Platform::LibInputHandler",
    "ready",
    "",
    "capabilitiesChanged",
    "suspendedChanged",
    "keyboardCountChanged",
    "count",
    "pointerCountChanged",
    "touchCountChanged",
    "tabletCountChanged",
    "gestureCountChanged",
    "touchDeviceRegistered",
    "QPointingDevice*",
    "td",
    "touchDeviceUnregistered",
    "keyPressed",
    "Originull::Platform::LibInputKeyEvent",
    "event",
    "keyReleased",
    "mousePressed",
    "Originull::Platform::LibInputMouseEvent",
    "mouseReleased",
    "mouseMoved",
    "mouseWheel",
    "touchEvent",
    "Originull::Platform::LibInputTouchEvent",
    "touchCancel",
    "suspend",
    "resume",
    "handleEvents",
    "suspended"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       1,  193, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  140,    2, 0x06,    2 /* Public */,
       3,    0,  141,    2, 0x06,    3 /* Public */,
       4,    1,  142,    2, 0x06,    4 /* Public */,
       5,    1,  145,    2, 0x06,    6 /* Public */,
       7,    1,  148,    2, 0x06,    8 /* Public */,
       8,    1,  151,    2, 0x06,   10 /* Public */,
       9,    1,  154,    2, 0x06,   12 /* Public */,
      10,    1,  157,    2, 0x06,   14 /* Public */,
      11,    1,  160,    2, 0x06,   16 /* Public */,
      14,    1,  163,    2, 0x06,   18 /* Public */,
      15,    1,  166,    2, 0x06,   20 /* Public */,
      18,    1,  169,    2, 0x06,   22 /* Public */,
      19,    1,  172,    2, 0x06,   24 /* Public */,
      21,    1,  175,    2, 0x06,   26 /* Public */,
      22,    1,  178,    2, 0x06,   28 /* Public */,
      23,    1,  181,    2, 0x06,   30 /* Public */,
      24,    1,  184,    2, 0x06,   32 /* Public */,
      26,    1,  187,    2, 0x06,   34 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      27,    0,  190,    2, 0x0a,   36 /* Public */,
      28,    0,  191,    2, 0x0a,   37 /* Public */,
      29,    0,  192,    2, 0x08,   38 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 20,   17,
    QMetaType::Void, 0x80000000 | 20,   17,
    QMetaType::Void, 0x80000000 | 20,   17,
    QMetaType::Void, 0x80000000 | 20,   17,
    QMetaType::Void, 0x80000000 | 25,   17,
    QMetaType::Void, 0x80000000 | 25,   17,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      30, QMetaType::Bool, 0x00015001, uint(2), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject Originull::Platform::LibInputHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS_t,
        // property 'suspended'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LibInputHandler, std::true_type>,
        // method 'ready'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'capabilitiesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'suspendedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'keyboardCountChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'pointerCountChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'touchCountChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'tabletCountChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'gestureCountChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'touchDeviceRegistered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPointingDevice *, std::false_type>,
        // method 'touchDeviceUnregistered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPointingDevice *, std::false_type>,
        // method 'keyPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputKeyEvent &, std::false_type>,
        // method 'keyReleased'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputKeyEvent &, std::false_type>,
        // method 'mousePressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputMouseEvent &, std::false_type>,
        // method 'mouseReleased'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputMouseEvent &, std::false_type>,
        // method 'mouseMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputMouseEvent &, std::false_type>,
        // method 'mouseWheel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputMouseEvent &, std::false_type>,
        // method 'touchEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputTouchEvent &, std::false_type>,
        // method 'touchCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Originull::Platform::LibInputTouchEvent &, std::false_type>,
        // method 'suspend'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleEvents'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Originull::Platform::LibInputHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LibInputHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ready(); break;
        case 1: _t->capabilitiesChanged(); break;
        case 2: _t->suspendedChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->keyboardCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->pointerCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->touchCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->tabletCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->gestureCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->touchDeviceRegistered((*reinterpret_cast< std::add_pointer_t<QPointingDevice*>>(_a[1]))); break;
        case 9: _t->touchDeviceUnregistered((*reinterpret_cast< std::add_pointer_t<QPointingDevice*>>(_a[1]))); break;
        case 10: _t->keyPressed((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputKeyEvent>>(_a[1]))); break;
        case 11: _t->keyReleased((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputKeyEvent>>(_a[1]))); break;
        case 12: _t->mousePressed((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputMouseEvent>>(_a[1]))); break;
        case 13: _t->mouseReleased((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputMouseEvent>>(_a[1]))); break;
        case 14: _t->mouseMoved((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputMouseEvent>>(_a[1]))); break;
        case 15: _t->mouseWheel((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputMouseEvent>>(_a[1]))); break;
        case 16: _t->touchEvent((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputTouchEvent>>(_a[1]))); break;
        case 17: _t->touchCancel((*reinterpret_cast< std::add_pointer_t<Originull::Platform::LibInputTouchEvent>>(_a[1]))); break;
        case 18: _t->suspend(); break;
        case 19: _t->resume(); break;
        case 20: _t->handleEvents(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QPointingDevice* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QPointingDevice* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputKeyEvent >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputKeyEvent >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputMouseEvent >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputMouseEvent >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputMouseEvent >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputMouseEvent >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputTouchEvent >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Originull::Platform::LibInputTouchEvent >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LibInputHandler::*)();
            if (_t _q_method = &LibInputHandler::ready; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)();
            if (_t _q_method = &LibInputHandler::capabilitiesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(bool );
            if (_t _q_method = &LibInputHandler::suspendedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(int );
            if (_t _q_method = &LibInputHandler::keyboardCountChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(int );
            if (_t _q_method = &LibInputHandler::pointerCountChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(int );
            if (_t _q_method = &LibInputHandler::touchCountChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(int );
            if (_t _q_method = &LibInputHandler::tabletCountChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(int );
            if (_t _q_method = &LibInputHandler::gestureCountChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(QPointingDevice * );
            if (_t _q_method = &LibInputHandler::touchDeviceRegistered; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(QPointingDevice * );
            if (_t _q_method = &LibInputHandler::touchDeviceUnregistered; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputKeyEvent & );
            if (_t _q_method = &LibInputHandler::keyPressed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputKeyEvent & );
            if (_t _q_method = &LibInputHandler::keyReleased; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputMouseEvent & );
            if (_t _q_method = &LibInputHandler::mousePressed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputMouseEvent & );
            if (_t _q_method = &LibInputHandler::mouseReleased; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputMouseEvent & );
            if (_t _q_method = &LibInputHandler::mouseMoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputMouseEvent & );
            if (_t _q_method = &LibInputHandler::mouseWheel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputTouchEvent & );
            if (_t _q_method = &LibInputHandler::touchEvent; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (LibInputHandler::*)(const Originull::Platform::LibInputTouchEvent & );
            if (_t _q_method = &LibInputHandler::touchCancel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<LibInputHandler *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isSuspended(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *Originull::Platform::LibInputHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Originull::Platform::LibInputHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSOriginullSCOPEPlatformSCOPELibInputHandlerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Originull::Platform::LibInputHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Originull::Platform::LibInputHandler::ready()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Originull::Platform::LibInputHandler::capabilitiesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Originull::Platform::LibInputHandler::suspendedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Originull::Platform::LibInputHandler::keyboardCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Originull::Platform::LibInputHandler::pointerCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Originull::Platform::LibInputHandler::touchCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Originull::Platform::LibInputHandler::tabletCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Originull::Platform::LibInputHandler::gestureCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Originull::Platform::LibInputHandler::touchDeviceRegistered(QPointingDevice * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Originull::Platform::LibInputHandler::touchDeviceUnregistered(QPointingDevice * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Originull::Platform::LibInputHandler::keyPressed(const Originull::Platform::LibInputKeyEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Originull::Platform::LibInputHandler::keyReleased(const Originull::Platform::LibInputKeyEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Originull::Platform::LibInputHandler::mousePressed(const Originull::Platform::LibInputMouseEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Originull::Platform::LibInputHandler::mouseReleased(const Originull::Platform::LibInputMouseEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Originull::Platform::LibInputHandler::mouseMoved(const Originull::Platform::LibInputMouseEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void Originull::Platform::LibInputHandler::mouseWheel(const Originull::Platform::LibInputMouseEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void Originull::Platform::LibInputHandler::touchEvent(const Originull::Platform::LibInputTouchEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void Originull::Platform::LibInputHandler::touchCancel(const Originull::Platform::LibInputTouchEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_WARNING_POP
