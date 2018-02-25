/****************************************************************************
** Meta object code from reading C++ file 'psk-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/psk-decoder/psk-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'psk-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_pskDecoder[] = {

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
      12,   11,   11,   11, 0x08,
      34,   11,   11,   11, 0x08,
      58,   11,   11,   11, 0x08,
      83,   11,   11,   11, 0x08,
     104,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_pskDecoder[] = {
    "pskDecoder\0\0psk_setAfcon(QString)\0"
    "psk_setReverse(QString)\0"
    "psk_setSquelchLevel(int)\0psk_setMode(QString)\0"
    "psk_setFilterDegree(int)\0"
};

void pskDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        pskDecoder *_t = static_cast<pskDecoder *>(_o);
        switch (_id) {
        case 0: _t->psk_setAfcon((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->psk_setReverse((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->psk_setSquelchLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->psk_setMode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->psk_setFilterDegree((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData pskDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject pskDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_pskDecoder,
      qt_meta_data_pskDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &pskDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *pskDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *pskDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_pskDecoder))
        return static_cast<void*>(const_cast< pskDecoder*>(this));
    if (!strcmp(_clname, "Ui_psk_widget"))
        return static_cast< Ui_psk_widget*>(const_cast< pskDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int pskDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
