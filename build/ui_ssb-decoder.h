/********************************************************************************
** Form generated from reading UI file 'ssb-decoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SSB_2D_DECODER_H
#define UI_SSB_2D_DECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ssb_decoder
{
public:
    QFrame *frame;
    QPushButton *adaptiveFilterButton;
    QSlider *lowpassFilterslider;
    QComboBox *submodeSelector;
    QSlider *adaptiveFilterSlider;

    void setupUi(QWidget *ssb_decoder)
    {
        if (ssb_decoder->objectName().isEmpty())
            ssb_decoder->setObjectName(QString::fromUtf8("ssb_decoder"));
        ssb_decoder->resize(360, 89);
        frame = new QFrame(ssb_decoder);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 351, 81));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        adaptiveFilterButton = new QPushButton(frame);
        adaptiveFilterButton->setObjectName(QString::fromUtf8("adaptiveFilterButton"));
        adaptiveFilterButton->setGeometry(QRect(20, 40, 121, 21));
        lowpassFilterslider = new QSlider(frame);
        lowpassFilterslider->setObjectName(QString::fromUtf8("lowpassFilterslider"));
        lowpassFilterslider->setGeometry(QRect(170, 10, 160, 24));
        lowpassFilterslider->setMaximum(100);
        lowpassFilterslider->setValue(100);
        lowpassFilterslider->setOrientation(Qt::Horizontal);
        submodeSelector = new QComboBox(frame);
        submodeSelector->setObjectName(QString::fromUtf8("submodeSelector"));
        submodeSelector->setGeometry(QRect(20, 10, 121, 31));
        adaptiveFilterSlider = new QSlider(frame);
        adaptiveFilterSlider->setObjectName(QString::fromUtf8("adaptiveFilterSlider"));
        adaptiveFilterSlider->setGeometry(QRect(170, 40, 160, 24));
        adaptiveFilterSlider->setOrientation(Qt::Horizontal);

        retranslateUi(ssb_decoder);

        QMetaObject::connectSlotsByName(ssb_decoder);
    } // setupUi

    void retranslateUi(QWidget *ssb_decoder)
    {
        ssb_decoder->setWindowTitle(QApplication::translate("ssb_decoder", "ssb decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        adaptiveFilterButton->setToolTip(QApplication::translate("ssb_decoder", "<html><head/><body><p>Adaptive noise filter can be set by this button. The slider then sets the degree.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        adaptiveFilterButton->setText(QApplication::translate("ssb_decoder", "filter", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lowpassFilterslider->setToolTip(QApplication::translate("ssb_decoder", "<html><head/><body><p>Lowpass filter, applied to the demodulated signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        submodeSelector->clear();
        submodeSelector->insertItems(0, QStringList()
         << QApplication::translate("ssb_decoder", "mode usb", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ssb_decoder", "mode lsb", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        submodeSelector->setToolTip(QApplication::translate("ssb_decoder", "<html><head/><body><p>Choose between upper- and lower side band demodulation. Note that an appropriate bandfilter is selected elsewhere.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        adaptiveFilterSlider->setToolTip(QApplication::translate("ssb_decoder", "<html><head/><body><p>adaptive filter depth setting.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class ssb_decoder: public Ui_ssb_decoder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SSB_2D_DECODER_H
