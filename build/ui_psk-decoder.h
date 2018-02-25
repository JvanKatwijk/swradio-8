/********************************************************************************
** Form generated from reading UI file 'psk-decoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PSK_2D_DECODER_H
#define UI_PSK_2D_DECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include "qwt_dial.h"

QT_BEGIN_NAMESPACE

class Ui_psk_widget
{
public:
    QFrame *frame;
    QwtDial *pskPhaseDisplay;
    QLCDNumber *pskIFdisplay;
    QLCDNumber *pskQualitydisplay;
    QComboBox *pskAfconTrigger;
    QComboBox *pskReverseTrigger;
    QComboBox *pskModeTrigger;
    QSpinBox *pskSquelchLevelTrigger;
    QSpinBox *pskFilterDegreeTrigger;
    QLabel *psktextBox;
    QLabel *label;

    void setupUi(QWidget *psk_widget)
    {
        if (psk_widget->objectName().isEmpty())
            psk_widget->setObjectName(QString::fromUtf8("psk_widget"));
        psk_widget->resize(595, 140);
        frame = new QFrame(psk_widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 591, 141));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        pskPhaseDisplay = new QwtDial(frame);
        pskPhaseDisplay->setObjectName(QString::fromUtf8("pskPhaseDisplay"));
        pskPhaseDisplay->setGeometry(QRect(10, 0, 131, 111));
        pskPhaseDisplay->setLineWidth(4);
        pskIFdisplay = new QLCDNumber(frame);
        pskIFdisplay->setObjectName(QString::fromUtf8("pskIFdisplay"));
        pskIFdisplay->setGeometry(QRect(160, 10, 64, 23));
        pskIFdisplay->setSegmentStyle(QLCDNumber::Flat);
        pskQualitydisplay = new QLCDNumber(frame);
        pskQualitydisplay->setObjectName(QString::fromUtf8("pskQualitydisplay"));
        pskQualitydisplay->setGeometry(QRect(160, 40, 64, 23));
        pskQualitydisplay->setSegmentStyle(QLCDNumber::Flat);
        pskAfconTrigger = new QComboBox(frame);
        pskAfconTrigger->setObjectName(QString::fromUtf8("pskAfconTrigger"));
        pskAfconTrigger->setGeometry(QRect(240, 10, 81, 21));
        pskReverseTrigger = new QComboBox(frame);
        pskReverseTrigger->setObjectName(QString::fromUtf8("pskReverseTrigger"));
        pskReverseTrigger->setGeometry(QRect(320, 10, 81, 21));
        pskModeTrigger = new QComboBox(frame);
        pskModeTrigger->setObjectName(QString::fromUtf8("pskModeTrigger"));
        pskModeTrigger->setGeometry(QRect(400, 10, 81, 21));
        pskSquelchLevelTrigger = new QSpinBox(frame);
        pskSquelchLevelTrigger->setObjectName(QString::fromUtf8("pskSquelchLevelTrigger"));
        pskSquelchLevelTrigger->setGeometry(QRect(240, 40, 81, 21));
        pskSquelchLevelTrigger->setMinimum(3);
        pskFilterDegreeTrigger = new QSpinBox(frame);
        pskFilterDegreeTrigger->setObjectName(QString::fromUtf8("pskFilterDegreeTrigger"));
        pskFilterDegreeTrigger->setGeometry(QRect(330, 40, 81, 21));
        pskFilterDegreeTrigger->setMinimum(10);
        pskFilterDegreeTrigger->setValue(12);
        psktextBox = new QLabel(frame);
        psktextBox->setObjectName(QString::fromUtf8("psktextBox"));
        psktextBox->setGeometry(QRect(0, 110, 591, 21));
        psktextBox->setFrameShape(QFrame::Panel);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(420, 30, 161, 41));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);

        retranslateUi(psk_widget);

        QMetaObject::connectSlotsByName(psk_widget);
    } // setupUi

    void retranslateUi(QWidget *psk_widget)
    {
        psk_widget->setWindowTitle(QApplication::translate("psk_widget", "psk-decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pskPhaseDisplay->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Phase difference. In a PSK31 signal, the phase difference between successive encoded bits is either 0 or 180 degrees. For QPSK phase differences may be a multiple of 90 degrees. The &quot;meter&quot; here shows (some) phasedifferences, that can be used to interpret the signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pskIFdisplay->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>pskIF display. Normally 800, if the AFc is set, this will vary. Can be used to finetune to the signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pskQualitydisplay->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Quality indicator. basically: the higher the number the better the quality of the signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pskAfconTrigger->clear();
        pskAfconTrigger->insertItems(0, QStringList()
         << QApplication::translate("psk_widget", "Afc off", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "Afc on", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        pskAfconTrigger->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Afc on or off. Note that the Agc only functions when there is a - reasonable - psk signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pskReverseTrigger->clear();
        pskReverseTrigger->insertItems(0, QStringList()
         << QApplication::translate("psk_widget", "normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "reverse", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        pskReverseTrigger->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Normal or reverse bit ordering in the psk &quot;words&quot;.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pskModeTrigger->clear();
        pskModeTrigger->insertItems(0, QStringList()
         << QApplication::translate("psk_widget", "psk31", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "psk63", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "qpsk31", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "qpsk63", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "psk125", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("psk_widget", "qpsk125", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        pskModeTrigger->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Select the psk &quot;mode&quot; here.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pskSquelchLevelTrigger->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Squelch level. I.e. below this level it is assumed there is no signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pskFilterDegreeTrigger->setToolTip(QApplication::translate("psk_widget", "<html><head/><body><p>Smallband filter. The order of the filter used to isolate the psk signal from the surrounding &quot;noise&quot;. Note that too high orders affect the signal in a negative way.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        psktextBox->setText(QString());
        label->setText(QApplication::translate("psk_widget", "psk decoder", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class psk_widget: public Ui_psk_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PSK_2D_DECODER_H
