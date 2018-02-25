/****************************************************************************
** Meta object code from reading C++ file 'cw-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/cw-decoder/cw-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cw-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_cwDecoder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      37,   10,   10,   10, 0x08,
      54,   10,   10,   10, 0x08,
      78,   10,   10,   10, 0x08,
     102,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_cwDecoder[] = {
    "cwDecoder\0\0cw_setWordsperMinute(int)\0"
    "cw_setTracking()\0cw_setFilterDegree(int)\0"
    "cw_setSquelchValue(int)\0cw_adjustFrequency(int)\0"
};

void cwDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        cwDecoder *_t = static_cast<cwDecoder *>(_o);
        switch (_id) {
        case 0: _t->cw_setWordsperMinute((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->cw_setTracking(); break;
        case 2: _t->cw_setFilterDegree((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->cw_setSquelchValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->cw_adjustFrequency((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData cwDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject cwDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_cwDecoder,
      qt_meta_data_cwDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &cwDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *cwDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *cwDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_cwDecoder))
        return static_cast<void*>(const_cast< cwDecoder*>(this));
    if (!strcmp(_clname, "Ui_cwDecoder"))
        return static_cast< Ui_cwDecoder*>(const_cast< cwDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int cwDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualDecoder::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
