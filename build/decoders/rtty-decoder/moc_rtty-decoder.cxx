/****************************************************************************
** Meta object code from reading C++ file 'rtty-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/rtty-decoder/rtty-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rtty-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_rttyDecoder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      36,   12,   12,   12, 0x08,
      61,   12,   12,   12, 0x08,
      84,   12,   12,   12, 0x08,
     110,   12,   12,   12, 0x08,
     134,   12,   12,   12, 0x08,
     155,   12,   12,   12, 0x08,
     178,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_rttyDecoder[] = {
    "rttyDecoder\0\0set_rttyAfcon(QString)\0"
    "set_rttyReverse(QString)\0"
    "set_rttyShift(QString)\0set_rttyBaudrate(QString)\0"
    "set_rttyParity(QString)\0set_rttyMsb(QString)\0"
    "set_rttyNbits(QString)\0set_rttyStopbits(QString)\0"
};

void rttyDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        rttyDecoder *_t = static_cast<rttyDecoder *>(_o);
        switch (_id) {
        case 0: _t->set_rttyAfcon((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->set_rttyReverse((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->set_rttyShift((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->set_rttyBaudrate((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->set_rttyParity((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->set_rttyMsb((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->set_rttyNbits((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->set_rttyStopbits((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData rttyDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject rttyDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_rttyDecoder,
      qt_meta_data_rttyDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &rttyDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *rttyDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *rttyDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_rttyDecoder))
        return static_cast<void*>(const_cast< rttyDecoder*>(this));
    if (!strcmp(_clname, "Ui_rtty_widget"))
        return static_cast< Ui_rtty_widget*>(const_cast< rttyDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int rttyDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualDecoder::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
