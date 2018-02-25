/****************************************************************************
** Meta object code from reading C++ file 'radio.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../radio.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radio.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RadioInterface[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      32,   30,   16,   15, 0x08,
      88,   15,   15,   15, 0x08,
     112,   15,   15,   15, 0x08,
     137,   15,   15,   15, 0x08,
     153,   15,   15,   15, 0x08,
     175,   15,   15,   15, 0x08,
     213,   15,  197,   15, 0x08,
     236,   15,   15,   15, 0x08,
     258,  254,   15,   15, 0x08,
     284,   15,   15,   15, 0x08,
     304,   15,   15,   15, 0x08,
     329,   15,   15,   15, 0x08,
     353,   15,   15,   15, 0x08,
     370,   15,   15,   15, 0x08,
     385,   15,   15,   15, 0x08,
     400,   15,   15,   15, 0x08,
     420,   15,   15,   15, 0x08,
     438,   15,   15,   15, 0x08,
     466,   15,   15,   15, 0x08,
     487,   15,   15,   15, 0x08,
     510,   15,   15,   15, 0x08,
     533,   15,   15,   15, 0x08,
     546,   15,   15,   15, 0x08,
     570,  563,   15,   15, 0x0a,
     589,   30,   15,   15, 0x0a,
     611,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RadioInterface[] = {
    "RadioInterface\0\0virtualInput*\0,\0"
    "selectDevice(QString,RingBuffer<std::complex<float> >*)\0"
    "adjustFrequency_hz(int)\0"
    "adjustFrequency_khz(int)\0handle_myLine()\0"
    "set_hfscopeLevel(int)\0set_lfscopeLevel(int)\0"
    "virtualDecoder*\0selectDecoder(QString)\0"
    "setFrequency(int)\0idx\0setStreamOutSelector(int)\0"
    "handle_freqButton()\0wheelEvent(QWheelEvent*)\0"
    "set_mouseIncrement(int)\0setBand(QString)\0"
    "set_inMiddle()\0set_freqSave()\0"
    "handle_quitButton()\0setStart(QString)\0"
    "set_agcThresholdSlider(int)\0"
    "set_AGCMode(QString)\0switch_hfViewMode(int)\0"
    "switch_lfViewMode(int)\0updateTime()\0"
    "set_dumpButton()\0amount\0sampleHandler(int)\0"
    "processAudio(int,int)\0setDetectorMarker(int)\0"
};

void RadioInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RadioInterface *_t = static_cast<RadioInterface *>(_o);
        switch (_id) {
        case 0: { virtualInput* _r = _t->selectDevice((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< RingBuffer<std::complex<float> >*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< virtualInput**>(_a[0]) = _r; }  break;
        case 1: _t->adjustFrequency_hz((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->adjustFrequency_khz((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->handle_myLine(); break;
        case 4: _t->set_hfscopeLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->set_lfscopeLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: { virtualDecoder* _r = _t->selectDecoder((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< virtualDecoder**>(_a[0]) = _r; }  break;
        case 7: _t->setFrequency((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->setStreamOutSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->handle_freqButton(); break;
        case 10: _t->wheelEvent((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        case 11: _t->set_mouseIncrement((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->setBand((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->set_inMiddle(); break;
        case 14: _t->set_freqSave(); break;
        case 15: _t->handle_quitButton(); break;
        case 16: _t->setStart((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->set_agcThresholdSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->set_AGCMode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->switch_hfViewMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->switch_lfViewMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->updateTime(); break;
        case 22: _t->set_dumpButton(); break;
        case 23: _t->sampleHandler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->processAudio((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 25: _t->setDetectorMarker((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RadioInterface::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RadioInterface::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_RadioInterface,
      qt_meta_data_RadioInterface, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RadioInterface::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RadioInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RadioInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RadioInterface))
        return static_cast<void*>(const_cast< RadioInterface*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int RadioInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
