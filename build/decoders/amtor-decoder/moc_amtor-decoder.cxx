/****************************************************************************
** Meta object code from reading C++ file 'amtor-decoder.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../decoders/amtor-decoder/amtor-decoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'amtor-decoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_amtorDecoder[] = {

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
      14,   13,   13,   13, 0x08,
      38,   13,   13,   13, 0x08,
      64,   13,   13,   13, 0x08,
      91,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_amtorDecoder[] = {
    "amtorDecoder\0\0set_amtorAfcon(QString)\0"
    "set_amtorReverse(QString)\0"
    "set_amtorFecError(QString)\0"
    "set_amtorMessage(QString)\0"
};

void amtorDecoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        amtorDecoder *_t = static_cast<amtorDecoder *>(_o);
        switch (_id) {
        case 0: _t->set_amtorAfcon((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->set_amtorReverse((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->set_amtorFecError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->set_amtorMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData amtorDecoder::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject amtorDecoder::staticMetaObject = {
    { &virtualDecoder::staticMetaObject, qt_meta_stringdata_amtorDecoder,
      qt_meta_data_amtorDecoder, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &amtorDecoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *amtorDecoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *amtorDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_amtorDecoder))
        return static_cast<void*>(const_cast< amtorDecoder*>(this));
    if (!strcmp(_clname, "Ui_amtorwidget"))
        return static_cast< Ui_amtorwidget*>(const_cast< amtorDecoder*>(this));
    return virtualDecoder::qt_metacast(_clname);
}

int amtorDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
