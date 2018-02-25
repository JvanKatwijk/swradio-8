/****************************************************************************
** Meta object code from reading C++ file 'waterfall-scope.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../scopes-qwt6/waterfall-scope.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'waterfall-scope.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_waterfallScope[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      40,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_waterfallScope[] = {
    "waterfallScope\0\0leftMouseClick(QPointF)\0"
    "rightMouseClick(QPointF)\0"
};

void waterfallScope::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        waterfallScope *_t = static_cast<waterfallScope *>(_o);
        switch (_id) {
        case 0: _t->leftMouseClick((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 1: _t->rightMouseClick((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData waterfallScope::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject waterfallScope::staticMetaObject = {
    { &virtualScope::staticMetaObject, qt_meta_stringdata_waterfallScope,
      qt_meta_data_waterfallScope, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &waterfallScope::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *waterfallScope::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *waterfallScope::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_waterfallScope))
        return static_cast<void*>(const_cast< waterfallScope*>(this));
    if (!strcmp(_clname, "QwtPlotSpectrogram"))
        return static_cast< QwtPlotSpectrogram*>(const_cast< waterfallScope*>(this));
    return virtualScope::qt_metacast(_clname);
}

int waterfallScope::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualScope::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
