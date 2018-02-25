/********************************************************************************
** Form generated from reading UI file 'sdrplay-widget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRPLAY_2D_WIDGET_H
#define UI_SDRPLAY_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_sdrplayWidget
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *statusLabel;
    QLCDNumber *api_version;
    QSlider *gainSlider;
    QLCDNumber *gainDisplay;
    QCheckBox *agcControl;
    QSpinBox *ppmControl;
    QLabel *serialNumber;
    QComboBox *antennaSelector;

    void setupUi(QWidget *sdrplayWidget)
    {
        if (sdrplayWidget->objectName().isEmpty())
            sdrplayWidget->setObjectName(QString::fromUtf8("sdrplayWidget"));
        sdrplayWidget->resize(224, 263);
        frame = new QFrame(sdrplayWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 211, 251));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 80, 101, 21));
        statusLabel = new QLabel(frame);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setGeometry(QRect(16, 180, 121, 21));
        api_version = new QLCDNumber(frame);
        api_version->setObjectName(QString::fromUtf8("api_version"));
        api_version->setGeometry(QRect(20, 40, 91, 21));
        api_version->setLineWidth(0);
        api_version->setSegmentStyle(QLCDNumber::Flat);
        gainSlider = new QSlider(frame);
        gainSlider->setObjectName(QString::fromUtf8("gainSlider"));
        gainSlider->setGeometry(QRect(170, 29, 20, 161));
        gainSlider->setOrientation(Qt::Vertical);
        gainDisplay = new QLCDNumber(frame);
        gainDisplay->setObjectName(QString::fromUtf8("gainDisplay"));
        gainDisplay->setGeometry(QRect(160, 0, 41, 23));
        gainDisplay->setDigitCount(3);
        gainDisplay->setSegmentStyle(QLCDNumber::Flat);
        agcControl = new QCheckBox(frame);
        agcControl->setObjectName(QString::fromUtf8("agcControl"));
        agcControl->setGeometry(QRect(110, 130, 51, 25));
        ppmControl = new QSpinBox(frame);
        ppmControl->setObjectName(QString::fromUtf8("ppmControl"));
        ppmControl->setGeometry(QRect(10, 15, 81, 21));
        ppmControl->setMinimum(-200);
        ppmControl->setMaximum(200);
        serialNumber = new QLabel(frame);
        serialNumber->setObjectName(QString::fromUtf8("serialNumber"));
        serialNumber->setGeometry(QRect(10, 170, 141, 20));
        antennaSelector = new QComboBox(frame);
        antennaSelector->setObjectName(QString::fromUtf8("antennaSelector"));
        antennaSelector->setGeometry(QRect(10, 200, 181, 36));

        retranslateUi(sdrplayWidget);

        QMetaObject::connectSlotsByName(sdrplayWidget);
    } // setupUi

    void retranslateUi(QWidget *sdrplayWidget)
    {
        sdrplayWidget->setWindowTitle(QApplication::translate("sdrplayWidget", "SDRplay control", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("sdrplayWidget", "mirics-SDRplay", 0, QApplication::UnicodeUTF8));
        statusLabel->setText(QString());
        agcControl->setText(QApplication::translate("sdrplayWidget", "agc", 0, QApplication::UnicodeUTF8));
        serialNumber->setText(QString());
        antennaSelector->clear();
        antennaSelector->insertItems(0, QStringList()
         << QApplication::translate("sdrplayWidget", "Antenna A", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("sdrplayWidget", "Antenna B", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class sdrplayWidget: public Ui_sdrplayWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRPLAY_2D_WIDGET_H
