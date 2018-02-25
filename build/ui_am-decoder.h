/********************************************************************************
** Form generated from reading UI file 'am-decoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AM_2D_DECODER_H
#define UI_AM_2D_DECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_am_decoder
{
public:
    QFrame *frame;
    QPushButton *adaptiveFilterButton;
    QLabel *analogMode;
    QLCDNumber *signalStrength;
    QSlider *lowpassFilterslider;
    QSlider *adaptiveFilterSlider;

    void setupUi(QWidget *am_decoder)
    {
        if (am_decoder->objectName().isEmpty())
            am_decoder->setObjectName(QString::fromUtf8("am_decoder"));
        am_decoder->resize(367, 94);
        frame = new QFrame(am_decoder);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 351, 91));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        adaptiveFilterButton = new QPushButton(frame);
        adaptiveFilterButton->setObjectName(QString::fromUtf8("adaptiveFilterButton"));
        adaptiveFilterButton->setGeometry(QRect(10, 10, 101, 21));
        analogMode = new QLabel(frame);
        analogMode->setObjectName(QString::fromUtf8("analogMode"));
        analogMode->setGeometry(QRect(20, 100, 161, 21));
        signalStrength = new QLCDNumber(frame);
        signalStrength->setObjectName(QString::fromUtf8("signalStrength"));
        signalStrength->setGeometry(QRect(20, 40, 64, 23));
        signalStrength->setFrameShape(QFrame::NoFrame);
        signalStrength->setSegmentStyle(QLCDNumber::Flat);
        lowpassFilterslider = new QSlider(frame);
        lowpassFilterslider->setObjectName(QString::fromUtf8("lowpassFilterslider"));
        lowpassFilterslider->setGeometry(QRect(130, 10, 160, 24));
        lowpassFilterslider->setMaximum(100);
        lowpassFilterslider->setValue(100);
        lowpassFilterslider->setOrientation(Qt::Horizontal);
        adaptiveFilterSlider = new QSlider(frame);
        adaptiveFilterSlider->setObjectName(QString::fromUtf8("adaptiveFilterSlider"));
        adaptiveFilterSlider->setGeometry(QRect(130, 50, 160, 24));
        adaptiveFilterSlider->setOrientation(Qt::Horizontal);

        retranslateUi(am_decoder);

        QMetaObject::connectSlotsByName(am_decoder);
    } // setupUi

    void retranslateUi(QWidget *am_decoder)
    {
        am_decoder->setWindowTitle(QApplication::translate("am_decoder", "am decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        adaptiveFilterButton->setToolTip(QApplication::translate("am_decoder", "<html><head/><body><p>Adaptive noise filter can be set by this button. The slider then sets the degree.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        adaptiveFilterButton->setText(QApplication::translate("am_decoder", "filter", 0, QApplication::UnicodeUTF8));
        analogMode->setText(QString());
#ifndef QT_NO_TOOLTIP
        signalStrength->setToolTip(QApplication::translate("am_decoder", "<html><head/><body><p>Measured signal Strength.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class am_decoder: public Ui_am_decoder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AM_2D_DECODER_H
