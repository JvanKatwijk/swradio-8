/****************************************************************************
** Meta object code from reading C++ file 'fax-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/fax-decoder/fax-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fax-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_faxDecoder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      20,   11,   11,   11, 0x08,
      40,   11,   11,   11, 0x08,
      61,   11,   11,   11, 0x08,
      83,   11,   11,   11, 0x08,
     105,   11,   11,   11, 0x08,
     127,   11,   11,   11, 0x08,
     141,   11,   11,   11, 0x08,
     157,   11,   11,   11, 0x08,
     179,   11,   11,   11, 0x08,
     200,   11,   11,   11, 0x08,
     220,   11,   11,   11, 0x08,
     244,  242,   11,   11, 0x08,
     268,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_faxDecoder[] = {
    "faxDecoder\0\0reset()\0fax_setIOC(QString)\0"
    "fax_setMode(QString)\0fax_setPhase(QString)\0"
    "fax_setColor(QString)\0fax_setDeviation(int)\0"
    "fax_setSkip()\0fax_setLPM(int)\0"
    "fax_setstartFreq(int)\0fax_setstopFreq(int)\0"
    "fax_setCarrier(int)\0fax_setsavingonDone()\0"
    ",\0fax_setClicked(int,int)\0"
    "fax_setautoContinue()\0"
};

void faxDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        faxDecoder *_t = static_cast<faxDecoder *>(_o);
        switch (_id) {
        case 0: _t->reset(); break;
        case 1: _t->fax_setIOC((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->fax_setMode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->fax_setPhase((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->fax_setColor((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->fax_setDeviation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->fax_setSkip(); break;
        case 7: _t->fax_setLPM((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->fax_setstartFreq((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->fax_setstopFreq((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->fax_setCarrier((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->fax_setsavingonDone(); break;
        case 12: _t->fax_setClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->fax_setautoContinue(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData faxDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject faxDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_faxDecoder,
      qt_meta_data_faxDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &faxDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *faxDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *faxDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_faxDecoder))
        return static_cast<void*>(const_cast< faxDecoder*>(this));
    if (!strcmp(_clname, "Ui_fax_decoder"))
        return static_cast< Ui_fax_decoder*>(const_cast< faxDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int faxDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualDecoder::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
