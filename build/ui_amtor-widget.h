/********************************************************************************
** Form generated from reading UI file 'amtor-widget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AMTOR_2D_WIDGET_H
#define UI_AMTOR_2D_WIDGET_H

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

class Ui_amtorwidget
{
public:
    QFrame *frame;
    QLabel *amtortextBox;
    QLCDNumber *amtorFreqCorrection;
    QLCDNumber *amtorStrengthMeter;
    QComboBox *amtorAfconButton;
    QComboBox *amtorReverseButton;
    QComboBox *amtorFecErrorButton;
    QComboBox *amtorMessage;
    QLabel *label;

    void setupUi(QWidget *amtorwidget)
    {
        if (amtorwidget->objectName().isEmpty())
            amtorwidget->setObjectName(QString::fromUtf8("amtorwidget"));
        amtorwidget->resize(564, 97);
        frame = new QFrame(amtorwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 661, 91));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        amtortextBox = new QLabel(frame);
        amtortextBox->setObjectName(QString::fromUtf8("amtortextBox"));
        amtortextBox->setGeometry(QRect(10, 60, 541, 21));
        amtortextBox->setFrameShape(QFrame::Box);
        amtortextBox->setFrameShadow(QFrame::Plain);
        amtorFreqCorrection = new QLCDNumber(frame);
        amtorFreqCorrection->setObjectName(QString::fromUtf8("amtorFreqCorrection"));
        amtorFreqCorrection->setGeometry(QRect(10, 10, 64, 23));
        amtorFreqCorrection->setSegmentStyle(QLCDNumber::Flat);
        amtorStrengthMeter = new QLCDNumber(frame);
        amtorStrengthMeter->setObjectName(QString::fromUtf8("amtorStrengthMeter"));
        amtorStrengthMeter->setGeometry(QRect(10, 30, 64, 23));
        amtorStrengthMeter->setSegmentStyle(QLCDNumber::Flat);
        amtorAfconButton = new QComboBox(frame);
        amtorAfconButton->setObjectName(QString::fromUtf8("amtorAfconButton"));
        amtorAfconButton->setGeometry(QRect(80, 10, 85, 21));
        amtorReverseButton = new QComboBox(frame);
        amtorReverseButton->setObjectName(QString::fromUtf8("amtorReverseButton"));
        amtorReverseButton->setGeometry(QRect(170, 10, 85, 21));
        amtorFecErrorButton = new QComboBox(frame);
        amtorFecErrorButton->setObjectName(QString::fromUtf8("amtorFecErrorButton"));
        amtorFecErrorButton->setGeometry(QRect(260, 10, 85, 21));
        amtorMessage = new QComboBox(frame);
        amtorMessage->setObjectName(QString::fromUtf8("amtorMessage"));
        amtorMessage->setGeometry(QRect(350, 10, 85, 21));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(450, 10, 67, 21));

        retranslateUi(amtorwidget);

        QMetaObject::connectSlotsByName(amtorwidget);
    } // setupUi

    void retranslateUi(QWidget *amtorwidget)
    {
        amtorwidget->setWindowTitle(QApplication::translate("amtorwidget", "amtor-decoder", 0, QApplication::UnicodeUTF8));
        amtortextBox->setText(QApplication::translate("amtorwidget", "TextLabel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        amtorFreqCorrection->setToolTip(QApplication::translate("amtorwidget", "<html><head/><body><p>Frequency correction.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        amtorStrengthMeter->setToolTip(QApplication::translate("amtorwidget", "<html><head/><body><p>&quot;Strength&quot; meter for the AMTOR signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        amtorAfconButton->clear();
        amtorAfconButton->insertItems(0, QStringList()
         << QApplication::translate("amtorwidget", "afc off", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("amtorwidget", "afc on", 0, QApplication::UnicodeUTF8)
        );
        amtorReverseButton->clear();
        amtorReverseButton->insertItems(0, QStringList()
         << QApplication::translate("amtorwidget", "normal", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("amtorwidget", "reverse", 0, QApplication::UnicodeUTF8)
        );
        amtorFecErrorButton->clear();
        amtorFecErrorButton->insertItems(0, QStringList()
         << QApplication::translate("amtorwidget", "non strict", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("amtorwidget", "strict", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        amtorFecErrorButton->setToolTip(QApplication::translate("amtorwidget", "<html><head/><body><p>Standard or non-standard decoding.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        amtorMessage->clear();
        amtorMessage->insertItems(0, QStringList()
         << QApplication::translate("amtorwidget", "all text", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("amtorwidget", "message", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        amtorMessage->setToolTip(QApplication::translate("amtorwidget", "<html><head/><body><p>All texts (right or wrong) or just recognized messages.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("amtorwidget", "amtor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class amtorwidget: public Ui_amtorwidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AMTOR_2D_WIDGET_H
