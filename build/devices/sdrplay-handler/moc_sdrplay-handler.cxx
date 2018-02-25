/****************************************************************************
** Meta object code from reading C++ file 'sdrplay-handler.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../devices/sdrplay-handler/sdrplay-handler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_sdrplayHandler[] = {

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
      16,   15,   15,   15, 0x08,
      37,   15,   15,   15, 0x08,
      61,   15,   15,   15, 0x08,
      81,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_sdrplayHandler[] = {
    "sdrplayHandler\0\0setExternalGain(int)\0"
    "agcControl_toggled(int)\0set_ppmControl(int)\0"
    "set_antennaControl(QString)\0"
};

void sdrplayHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        sdrplayHandler *_t = static_cast<sdrplayHandler *>(_o);
        switch (_id) {
        case 0: _t->setExternalGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->agcControl_toggled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_ppmControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_antennaControl((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData sdrplayHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject sdrplayHandler::staticMetaObject = {
    { &virtualInput::staticMetaObject, qt_meta_stringdata_sdrplayHandler,
      qt_meta_data_sdrplayHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &sdrplayHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *sdrplayHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *sdrplayHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_sdrplayHandler))
        return static_cast<void*>(const_cast< sdrplayHandler*>(this));
    if (!strcmp(_clname, "Ui_sdrplayWidget"))
        return static_cast< Ui_sdrplayWidget*>(const_cast< sdrplayHandler*>(this));
    return virtualInput::qt_metacast(_clname);
}

int sdrplayHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualInput::qt_metacall(_c, _id, _a);
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
