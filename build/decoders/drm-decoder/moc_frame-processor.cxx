/****************************************************************************
** Meta object code from reading C++ file 'frame-processor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/drm-decoder/frame-processor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'frame-processor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_frameProcessor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      34,   15,   15,   15, 0x05,
      51,   15,   15,   15, 0x05,
      68,   15,   15,   15, 0x05,
      83,   15,   15,   15, 0x05,
     104,  102,   15,   15, 0x05,
     127,   15,   15,   15, 0x05,
     151,   15,   15,   15, 0x05,
     163,   15,   15,   15, 0x05,
     178,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_frameProcessor[] = {
    "frameProcessor\0\0setTimeSync(bool)\0"
    "setFACSync(bool)\0setSDCSync(bool)\0"
    "show_Mode(int)\0show_Spectrum(int)\0,\0"
    "show_services(int,int)\0show_audioMode(QString)\0"
    "showEq(int)\0showSNR(float)\0show_freq2(float)\0"
};

void frameProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        frameProcessor *_t = static_cast<frameProcessor *>(_o);
        switch (_id) {
        case 0: _t->setTimeSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setFACSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setSDCSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->show_Mode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->show_Spectrum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->show_services((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->show_audioMode((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->showEq((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->showSNR((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 9: _t->show_freq2((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData frameProcessor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject frameProcessor::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_frameProcessor,
      qt_meta_data_frameProcessor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &frameProcessor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *frameProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *frameProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_frameProcessor))
        return static_cast<void*>(const_cast< frameProcessor*>(this));
    return QThread::qt_metacast(_clname);
}

int frameProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void frameProcessor::setTimeSync(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void frameProcessor::setFACSync(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void frameProcessor::setSDCSync(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void frameProcessor::show_Mode(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void frameProcessor::show_Spectrum(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void frameProcessor::show_services(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void frameProcessor::show_audioMode(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void frameProcessor::showEq(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void frameProcessor::showSNR(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void frameProcessor::show_freq2(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
