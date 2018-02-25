/********************************************************************************
** Form generated from reading UI file 'rtty-decoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RTTY_2D_DECODER_H
#define UI_RTTY_2D_DECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rtty_widget
{
public:
    QFrame *frame;
    QLCDNumber *rttyStrengthMeter;
    QLCDNumber *rttyFreqCorrection;
    QLCDNumber *rttyBaudRate;
    QLCDNumber *rttyGuess;
    QLabel *rttytextBox;
    QComboBox *rttyWidthSelect;
    QComboBox *rttyBaudrateSelect;
    QComboBox *rttyParitySelect;
    QComboBox *rttyMsbSelect;
    QComboBox *rttyAfconTrigger;
    QComboBox *rttyReverseTrigger;
    QComboBox *rttyNbitsTrigger;
    QComboBox *rttyStopbitsTrigger;
    QLabel *label;

    void setupUi(QWidget *rtty_widget)
    {
        if (rtty_widget->objectName().isEmpty())
            rtty_widget->setObjectName(QString::fromUtf8("rtty_widget"));
        rtty_widget->resize(596, 139);
        frame = new QFrame(rtty_widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 661, 131));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        rttyStrengthMeter = new QLCDNumber(frame);
        rttyStrengthMeter->setObjectName(QString::fromUtf8("rttyStrengthMeter"));
        rttyStrengthMeter->setGeometry(QRect(20, 10, 64, 23));
        rttyStrengthMeter->setSegmentStyle(QLCDNumber::Flat);
        rttyFreqCorrection = new QLCDNumber(frame);
        rttyFreqCorrection->setObjectName(QString::fromUtf8("rttyFreqCorrection"));
        rttyFreqCorrection->setGeometry(QRect(20, 30, 64, 23));
        rttyFreqCorrection->setSegmentStyle(QLCDNumber::Flat);
        rttyBaudRate = new QLCDNumber(frame);
        rttyBaudRate->setObjectName(QString::fromUtf8("rttyBaudRate"));
        rttyBaudRate->setGeometry(QRect(20, 50, 64, 23));
        rttyBaudRate->setSegmentStyle(QLCDNumber::Flat);
        rttyGuess = new QLCDNumber(frame);
        rttyGuess->setObjectName(QString::fromUtf8("rttyGuess"));
        rttyGuess->setGeometry(QRect(20, 70, 64, 23));
        rttyGuess->setSegmentStyle(QLCDNumber::Flat);
        rttytextBox = new QLabel(frame);
        rttytextBox->setObjectName(QString::fromUtf8("rttytextBox"));
        rttytextBox->setGeometry(QRect(0, 100, 591, 21));
        rttytextBox->setFrameShape(QFrame::Box);
        rttyWidthSelect = new QComboBox(frame);
        rttyWidthSelect->setObjectName(QString::fromUtf8("rttyWidthSelect"));
        rttyWidthSelect->setGeometry(QRect(140, 10, 91, 21));
        rttyBaudrateSelect = new QComboBox(frame);
        rttyBaudrateSelect->setObjectName(QString::fromUtf8("rttyBaudrateSelect"));
        rttyBaudrateSelect->setGeometry(QRect(140, 30, 91, 21));
        rttyParitySelect = new QComboBox(frame);
        rttyParitySelect->setObjectName(QString::fromUtf8("rttyParitySelect"));
        rttyParitySelect->setGeometry(QRect(140, 50, 91, 21));
        rttyMsbSelect = new QComboBox(frame);
        rttyMsbSelect->setObjectName(QString::fromUtf8("rttyMsbSelect"));
        rttyMsbSelect->setGeometry(QRect(140, 70, 91, 21));
        rttyAfconTrigger = new QComboBox(frame);
        rttyAfconTrigger->setObjectName(QString::fromUtf8("rttyAfconTrigger"));
        rttyAfconTrigger->setGeometry(QRect(250, 10, 85, 21));
        rttyReverseTrigger = new QComboBox(frame);
        rttyReverseTrigger->setObjectName(QString::fromUtf8("rttyReverseTrigger"));
        rttyReverseTrigger->setGeometry(QRect(250, 30, 85, 21));
        rttyNbitsTrigger = new QComboBox(frame);
        rttyNbitsTrigger->setObjectName(QString::fromUtf8("rttyNbitsTrigger"));
        rttyNbitsTrigger->setGeometry(QRect(250, 50, 85, 21));
        rttyStopbitsTrigger = new QComboBox(frame);
        rttyStopbitsTrigger->setObjectName(QString::fromUtf8("rttyStopbitsTrigger"));
        rttyStopbitsTrigger->setGeometry(QRect(250, 70, 85, 21));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(430, 10, 151, 51));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);

        retranslateUi(rtty_widget);

        QMetaObject::connectSlotsByName(rtty_widget);
    } // setupUi

    void retranslateUi(QWidget *rtty_widget)
    {
        rtty_widget->setWindowTitle(QApplication::translate("rtty_widget", "rtty decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        rttyStrengthMeter->setToolTip(QApplication::translate("rtty_widget", "<html><head/><body><p>IF offset. The software tries to guess the offset of the tuned frequency.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rttyFreqCorrection->setToolTip(QApplication::translate("rtty_widget", "<html><head/><body><p>Frequency correction as applied if the Agc is on.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rttyBaudRate->setToolTip(QApplication::translate("rtty_widget", "<html><head/><body><p>Selected baudrate.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rttyGuess->setToolTip(QApplication::translate("rtty_widget", "<html><head/><body><p>Baudrate as guessed by the software.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        rttytextBox->setText(QApplication::translate("rtty_widget", "TextLabel", 0, QApplication::UnicodeUTF8));
        rttyWidthSelect->clear();
        rttyWidthSelect->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "170", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "225", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "300", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "450", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "850", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "900", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "1200", 0, QApplication::UnicodeUTF8)
        );
        rttyBaudrateSelect->clear();
        rttyBaudrateSelect->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "45", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "50", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "75", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "100", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "300", 0, QApplication::UnicodeUTF8)
        );
        rttyParitySelect->clear();
        rttyParitySelect->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "par none", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "par one", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "par odd", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "par even", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "par zero", 0, QApplication::UnicodeUTF8)
        );
        rttyMsbSelect->clear();
        rttyMsbSelect->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "msb false", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "msb true", 0, QApplication::UnicodeUTF8)
        );
        rttyAfconTrigger->clear();
        rttyAfconTrigger->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "afc off", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "afc on", 0, QApplication::UnicodeUTF8)
        );
        rttyReverseTrigger->clear();
        rttyReverseTrigger->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "reverse", 0, QApplication::UnicodeUTF8)
        );
        rttyNbitsTrigger->clear();
        rttyNbitsTrigger->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "5 bits", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "7 bits", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "8 bits", 0, QApplication::UnicodeUTF8)
        );
        rttyStopbitsTrigger->clear();
        rttyStopbitsTrigger->insertItems(0, QStringList()
         << QApplication::translate("rtty_widget", "1 stop", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "1.5 stop", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("rtty_widget", "2 stop", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("rtty_widget", "rtty decoder", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class rtty_widget: public Ui_rtty_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RTTY_2D_DECODER_H
