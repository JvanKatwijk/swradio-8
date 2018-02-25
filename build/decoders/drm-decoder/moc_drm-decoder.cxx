/****************************************************************************
** Meta object code from reading C++ file 'drm-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/drm-decoder/drm-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drm-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_drmDecoder[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      38,   36,   11,   11, 0x0a,
      61,   11,   11,   11, 0x0a,
      86,   11,   11,   11, 0x0a,
     109,   11,   11,   11, 0x0a,
     131,   11,   11,   11, 0x0a,
     154,   11,   11,   11, 0x0a,
     178,   36,   11,   11, 0x0a,
     201,   11,   11,   11, 0x0a,
     219,   11,   11,   11, 0x0a,
     237,   36,   11,   11, 0x0a,
     261,   11,   11,   11, 0x08,
     283,   11,   11,   11, 0x08,
     304,   11,   11,   11, 0x08,
     325,   11,   11,   11, 0x08,
     347,   11,   11,   11, 0x08,
     373,   11,   11,   11, 0x08,
     388,   11,   11,   11, 0x08,
     409,   11,   11,   11, 0x08,
     427,   11,   11,   11, 0x08,
     445,   11,   11,   11, 0x08,
     463,   11,   11,   11, 0x08,
     481,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_drmDecoder[] = {
    "drmDecoder\0\0show_audioMode(QString)\0"
    ",\0sampleOut(float,float)\0"
    "show_coarseOffset(float)\0"
    "show_fineOffset(float)\0show_timeDelay(float)\0"
    "show_timeOffset(float)\0show_clockOffset(float)\0"
    "show_channels(int,int)\0show_angle(float)\0"
    "faadSuccess(bool)\0showMOT(QByteArray,int)\0"
    "executeTimeSync(bool)\0executeFACSync(bool)\0"
    "executeSDCSync(bool)\0execute_showMode(int)\0"
    "execute_showSpectrum(int)\0showSNR(float)\0"
    "showMessage(QString)\0selectChannel_1()\0"
    "selectChannel_2()\0selectChannel_3()\0"
    "selectChannel_4()\0set_phaseOffset(int)\0"
};

void drmDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        drmDecoder *_t = static_cast<drmDecoder *>(_o);
        switch (_id) {
        case 0: _t->show_audioMode((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->sampleOut((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 2: _t->show_coarseOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: _t->show_fineOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: _t->show_timeDelay((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->show_timeOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 6: _t->show_clockOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 7: _t->show_channels((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->show_angle((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 9: _t->faadSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->showMOT((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->executeTimeSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->executeFACSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->executeSDCSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->execute_showMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->execute_showSpectrum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->showSNR((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 17: _t->showMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->selectChannel_1(); break;
        case 19: _t->selectChannel_2(); break;
        case 20: _t->selectChannel_3(); break;
        case 21: _t->selectChannel_4(); break;
        case 22: _t->set_phaseOffset((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData drmDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject drmDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_drmDecoder,
      qt_meta_data_drmDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &drmDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *drmDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *drmDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_drmDecoder))
        return static_cast<void*>(const_cast< drmDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int drmDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualDecoder::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
