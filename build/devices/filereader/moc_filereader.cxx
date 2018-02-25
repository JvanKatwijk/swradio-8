/****************************************************************************
** Meta object code from reading C++ file 'filereader.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../devices/filereader/filereader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filereader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_fileReader[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      20,   11,   11,   11, 0x08,
      44,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_fileReader[] = {
    "fileReader\0\0reset()\0handle_progressBar(int)\0"
    "set_progressBar(int)\0"
};

void fileReader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        fileReader *_t = static_cast<fileReader *>(_o);
        switch (_id) {
        case 0: _t->reset(); break;
        case 1: _t->handle_progressBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_progressBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData fileReader::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject fileReader::staticMetaObject = {
    { &virtualInput::staticMetaObject, qt_meta_stringdata_fileReader,
      qt_meta_data_fileReader, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &fileReader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *fileReader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *fileReader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_fileReader))
        return static_cast<void*>(const_cast< fileReader*>(this));
    if (!strcmp(_clname, "Ui_filereader"))
        return static_cast< Ui_filereader*>(const_cast< fileReader*>(this));
    return virtualInput::qt_metacast(_clname);
}

int fileReader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = virtualInput::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
