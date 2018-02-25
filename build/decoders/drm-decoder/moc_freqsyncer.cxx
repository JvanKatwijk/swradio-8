/****************************************************************************
** Meta object code from reading C++ file 'freqsyncer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/drm-decoder/freqsyncer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'freqsyncer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_freqSyncer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      37,   11,   11,   11, 0x05,
      60,   11,   11,   11, 0x05,
      83,   11,   11,   11, 0x05,
     105,   11,   11,   11, 0x05,
     129,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_freqSyncer[] = {
    "freqSyncer\0\0show_coarseOffset(float)\0"
    "show_fineOffset(float)\0show_timeOffset(float)\0"
    "show_timeDelay(float)\0show_clockOffset(float)\0"
    "show_angle(float)\0"
};

void freqSyncer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        freqSyncer *_t = static_cast<freqSyncer *>(_o);
        switch (_id) {
        case 0: _t->show_coarseOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->show_fineOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->show_timeOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: _t->show_timeDelay((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: _t->show_clockOffset((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->show_angle((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData freqSyncer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject freqSyncer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_freqSyncer,
      qt_meta_data_freqSyncer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &freqSyncer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *freqSyncer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *freqSyncer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_freqSyncer))
        return static_cast<void*>(const_cast< freqSyncer*>(this));
    return QObject::qt_metacast(_clname);
}

int freqSyncer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void freqSyncer::show_coarseOffset(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void freqSyncer::show_fineOffset(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void freqSyncer::show_timeOffset(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void freqSyncer::show_timeDelay(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void freqSyncer::show_clockOffset(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void freqSyncer::show_angle(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
