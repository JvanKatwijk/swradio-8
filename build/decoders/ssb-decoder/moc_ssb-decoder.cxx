/****************************************************************************
** Meta object code from reading C++ file 'ssb-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/ssb-decoder/ssb-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ssb-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ssbDecoder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      33,   11,   11,   11, 0x08,
      61,   11,   11,   11, 0x08,
      84,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ssbDecoder[] = {
    "ssbDecoder\0\0set_adaptiveFilter()\0"
    "set_adaptiveFiltersize(int)\0"
    "set_lowpassFilter(int)\0set_mode(QString)\0"
};

void ssbDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ssbDecoder *_t = static_cast<ssbDecoder *>(_o);
        switch (_id) {
        case 0: _t->set_adaptiveFilter(); break;
        case 1: _t->set_adaptiveFiltersize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_lowpassFilter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_mode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ssbDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ssbDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_ssbDecoder,
      qt_meta_data_ssbDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ssbDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ssbDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ssbDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ssbDecoder))
        return static_cast<void*>(const_cast< ssbDecoder*>(this));
    if (!strcmp(_clname, "Ui_ssb_decoder"))
        return static_cast< Ui_ssb_decoder*>(const_cast< ssbDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int ssbDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualDecoder::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
