/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Wed 2. Jun 21:45:25 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
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
      37,   11,   11,   11, 0x08,
      63,   11,   11,   11, 0x08,
      89,   11,   11,   11, 0x08,
     116,   11,   11,   11, 0x08,
     142,   11,   11,   11, 0x08,
     167,   11,   11,   11, 0x08,
     192,   11,   11,   11, 0x08,
     218,   11,   11,   11, 0x08,
     244,   11,   11,   11, 0x08,
     257,   11,   11,   11, 0x08,
     271,   11,   11,   11, 0x08,
     286,   11,   11,   11, 0x08,
     303,  301,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_actionM2M_triggered()\0"
    "on_actionEcho_triggered()\0"
    "on_actionRear_triggered()\0"
    "on_actionFront_triggered()\0"
    "on_actionHigh_triggered()\0"
    "on_actionMid_triggered()\0"
    "on_actionLow_triggered()\0"
    "on_actionHalf_triggered()\0"
    "on_actionFull_triggered()\0helpAction()\0"
    "aboutAction()\0DebugDecoder()\0"
    "DebugEncoder()\0p\0OnRefresh(unsigned char*)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_actionM2M_triggered(); break;
        case 1: on_actionEcho_triggered(); break;
        case 2: on_actionRear_triggered(); break;
        case 3: on_actionFront_triggered(); break;
        case 4: on_actionHigh_triggered(); break;
        case 5: on_actionMid_triggered(); break;
        case 6: on_actionLow_triggered(); break;
        case 7: on_actionHalf_triggered(); break;
        case 8: on_actionFull_triggered(); break;
        case 9: helpAction(); break;
        case 10: aboutAction(); break;
        case 11: DebugDecoder(); break;
        case 12: DebugEncoder(); break;
        case 13: OnRefresh((*reinterpret_cast< unsigned char*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
