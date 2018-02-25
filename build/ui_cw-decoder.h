/********************************************************************************
** Form generated from reading UI file 'cw-decoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CW_2D_DECODER_H
#define UI_CW_2D_DECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cwDecoder
{
public:
    QFrame *frame;
    QLabel *cwCharbox;
    QLabel *cwTextbox;
    QSpinBox *SquelchLevel;
    QSpinBox *WPM;
    QSpinBox *FilterDegree;
    QLCDNumber *actualWPM;
    QPushButton *Tracker;
    QLCDNumber *noiseLeveldisplay;
    QLCDNumber *agcPeakdisplay;
    QLCDNumber *spaceLengthdisplay;
    QLCDNumber *dotLengthdisplay;

    void setupUi(QWidget *cwDecoder)
    {
        if (cwDecoder->objectName().isEmpty())
            cwDecoder->setObjectName(QString::fromUtf8("cwDecoder"));
        cwDecoder->resize(532, 112);
        frame = new QFrame(cwDecoder);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 0, 521, 101));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        cwCharbox = new QLabel(frame);
        cwCharbox->setObjectName(QString::fromUtf8("cwCharbox"));
        cwCharbox->setGeometry(QRect(90, 30, 101, 21));
        cwCharbox->setFrameShape(QFrame::Box);
        cwCharbox->setLineWidth(2);
        cwTextbox = new QLabel(frame);
        cwTextbox->setObjectName(QString::fromUtf8("cwTextbox"));
        cwTextbox->setGeometry(QRect(10, 10, 491, 21));
        cwTextbox->setFrameShape(QFrame::Box);
        SquelchLevel = new QSpinBox(frame);
        SquelchLevel->setObjectName(QString::fromUtf8("SquelchLevel"));
        SquelchLevel->setGeometry(QRect(10, 30, 71, 21));
        SquelchLevel->setValue(5);
        WPM = new QSpinBox(frame);
        WPM->setObjectName(QString::fromUtf8("WPM"));
        WPM->setGeometry(QRect(10, 50, 71, 21));
        WPM->setValue(30);
        FilterDegree = new QSpinBox(frame);
        FilterDegree->setObjectName(QString::fromUtf8("FilterDegree"));
        FilterDegree->setGeometry(QRect(10, 70, 71, 21));
        FilterDegree->setValue(12);
        actualWPM = new QLCDNumber(frame);
        actualWPM->setObjectName(QString::fromUtf8("actualWPM"));
        actualWPM->setGeometry(QRect(320, 30, 64, 20));
        actualWPM->setSegmentStyle(QLCDNumber::Flat);
        Tracker = new QPushButton(frame);
        Tracker->setObjectName(QString::fromUtf8("Tracker"));
        Tracker->setGeometry(QRect(90, 60, 81, 21));
        noiseLeveldisplay = new QLCDNumber(frame);
        noiseLeveldisplay->setObjectName(QString::fromUtf8("noiseLeveldisplay"));
        noiseLeveldisplay->setGeometry(QRect(380, 30, 64, 23));
        noiseLeveldisplay->setSegmentStyle(QLCDNumber::Flat);
        agcPeakdisplay = new QLCDNumber(frame);
        agcPeakdisplay->setObjectName(QString::fromUtf8("agcPeakdisplay"));
        agcPeakdisplay->setGeometry(QRect(440, 30, 64, 23));
        agcPeakdisplay->setSegmentStyle(QLCDNumber::Flat);
        spaceLengthdisplay = new QLCDNumber(frame);
        spaceLengthdisplay->setObjectName(QString::fromUtf8("spaceLengthdisplay"));
        spaceLengthdisplay->setGeometry(QRect(190, 30, 64, 23));
        spaceLengthdisplay->setSegmentStyle(QLCDNumber::Flat);
        dotLengthdisplay = new QLCDNumber(frame);
        dotLengthdisplay->setObjectName(QString::fromUtf8("dotLengthdisplay"));
        dotLengthdisplay->setGeometry(QRect(250, 30, 64, 23));
        dotLengthdisplay->setSegmentStyle(QLCDNumber::Flat);

        retranslateUi(cwDecoder);

        QMetaObject::connectSlotsByName(cwDecoder);
    } // setupUi

    void retranslateUi(QWidget *cwDecoder)
    {
        cwDecoder->setWindowTitle(QApplication::translate("cwDecoder", "cw-decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cwCharbox->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>The &quot;dots&quot;, &quot;dashes&quot; and &quot;spaces&quot; in the current word.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cwCharbox->setText(QString());
        cwTextbox->setText(QString());
#ifndef QT_NO_TOOLTIP
        SquelchLevel->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Squelch level, signal strength below this level is assumed to be noise.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        WPM->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Word length guess. Used to guess the length of &quot;dots&quot;, &quot;dashed&quot; and &quot;spaces&quot;. </p><p>Most amateur transmissions are synchronized on with the default selection of 30.</p><p>Fast transmissions - noted by the fact that everything becomes a &quot;dot&quot;, need a higher setting.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        FilterDegree->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Order of the filter that is used to isolate the selected signal from the surrounding &quot;noise&quot;.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        actualWPM->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Guess of the wordlength.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Tracker->setText(QApplication::translate("cwDecoder", " Tracker on", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        noiseLeveldisplay->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Average noise Level.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        agcPeakdisplay->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Signal peak level.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        spaceLengthdisplay->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Guess of the duration of a &quot;space&quot; (in samples).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        dotLengthdisplay->setToolTip(QApplication::translate("cwDecoder", "<html><head/><body><p>Guess of the length of &quot;dots&quot; (in samples).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class cwDecoder: public Ui_cwDecoder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CW_2D_DECODER_H
