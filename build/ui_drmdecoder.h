/********************************************************************************
** Form generated from reading UI file 'drmdecoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DRMDECODER_H
#define UI_DRMDECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_drmdecoder
{
public:
    QFrame *frame;
    QLCDNumber *show_int_offset;
    QLCDNumber *show_small_offset;
    QLabel *timeSyncLabel;
    QLabel *label_2;
    QLabel *facSyncLabel;
    QLabel *label_3;
    QLabel *modeIndicator;
    QLabel *sdcSyncLabel;
    QLabel *label_4;
    QLabel *stationLabel;
    QLabel *audioModelabel;
    QLCDNumber *snrDisplay;
    QLabel *spectrumIndicator;
    QPushButton *channel_1;
    QPushButton *channel_2;
    QPushButton *channel_3;
    QPushButton *channel_4;
    QLCDNumber *timeOffsetDisplay;
    QLCDNumber *clockOffsetDisplay;
    QLCDNumber *timeDelayDisplay;
    QLCDNumber *angleDisplay;
    QLabel *faadSyncLabel;
    QLabel *messageLabel;
    QLCDNumber *phaseOffsetDisplay;

    void setupUi(QWidget *drmdecoder)
    {
        if (drmdecoder->objectName().isEmpty())
            drmdecoder->setObjectName(QString::fromUtf8("drmdecoder"));
        drmdecoder->resize(454, 133);
        frame = new QFrame(drmdecoder);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 441, 121));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        show_int_offset = new QLCDNumber(frame);
        show_int_offset->setObjectName(QString::fromUtf8("show_int_offset"));
        show_int_offset->setGeometry(QRect(80, 10, 41, 21));
        show_int_offset->setFrameShape(QFrame::NoFrame);
        show_int_offset->setSegmentStyle(QLCDNumber::Flat);
        show_small_offset = new QLCDNumber(frame);
        show_small_offset->setObjectName(QString::fromUtf8("show_small_offset"));
        show_small_offset->setGeometry(QRect(80, 30, 41, 21));
        show_small_offset->setFrameShape(QFrame::NoFrame);
        show_small_offset->setSegmentStyle(QLCDNumber::Flat);
        timeSyncLabel = new QLabel(frame);
        timeSyncLabel->setObjectName(QString::fromUtf8("timeSyncLabel"));
        timeSyncLabel->setGeometry(QRect(390, 10, 31, 21));
        timeSyncLabel->setFrameShape(QFrame::Box);
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(320, 10, 66, 21));
        facSyncLabel = new QLabel(frame);
        facSyncLabel->setObjectName(QString::fromUtf8("facSyncLabel"));
        facSyncLabel->setGeometry(QRect(390, 30, 31, 21));
        facSyncLabel->setFrameShape(QFrame::Box);
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(320, 30, 66, 21));
        modeIndicator = new QLabel(frame);
        modeIndicator->setObjectName(QString::fromUtf8("modeIndicator"));
        modeIndicator->setGeometry(QRect(200, 50, 31, 21));
        modeIndicator->setFrameShape(QFrame::Box);
        sdcSyncLabel = new QLabel(frame);
        sdcSyncLabel->setObjectName(QString::fromUtf8("sdcSyncLabel"));
        sdcSyncLabel->setGeometry(QRect(390, 50, 31, 21));
        sdcSyncLabel->setFrameShape(QFrame::Box);
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(320, 50, 64, 21));
        stationLabel = new QLabel(frame);
        stationLabel->setObjectName(QString::fromUtf8("stationLabel"));
        stationLabel->setGeometry(QRect(170, 70, 131, 21));
        stationLabel->setFrameShape(QFrame::Box);
        stationLabel->setLineWidth(1);
        audioModelabel = new QLabel(frame);
        audioModelabel->setObjectName(QString::fromUtf8("audioModelabel"));
        audioModelabel->setGeometry(QRect(230, 50, 71, 20));
        audioModelabel->setFrameShape(QFrame::Box);
        snrDisplay = new QLCDNumber(frame);
        snrDisplay->setObjectName(QString::fromUtf8("snrDisplay"));
        snrDisplay->setGeometry(QRect(70, 70, 51, 23));
        snrDisplay->setFrameShape(QFrame::NoFrame);
        snrDisplay->setFrameShadow(QFrame::Plain);
        snrDisplay->setLineWidth(0);
        snrDisplay->setSegmentStyle(QLCDNumber::Flat);
        spectrumIndicator = new QLabel(frame);
        spectrumIndicator->setObjectName(QString::fromUtf8("spectrumIndicator"));
        spectrumIndicator->setGeometry(QRect(170, 50, 31, 21));
        spectrumIndicator->setFrameShape(QFrame::Box);
        channel_1 = new QPushButton(frame);
        channel_1->setObjectName(QString::fromUtf8("channel_1"));
        channel_1->setGeometry(QRect(140, 10, 41, 31));
        channel_2 = new QPushButton(frame);
        channel_2->setObjectName(QString::fromUtf8("channel_2"));
        channel_2->setGeometry(QRect(180, 10, 41, 31));
        channel_3 = new QPushButton(frame);
        channel_3->setObjectName(QString::fromUtf8("channel_3"));
        channel_3->setGeometry(QRect(220, 10, 41, 31));
        channel_4 = new QPushButton(frame);
        channel_4->setObjectName(QString::fromUtf8("channel_4"));
        channel_4->setGeometry(QRect(260, 10, 41, 31));
        timeOffsetDisplay = new QLCDNumber(frame);
        timeOffsetDisplay->setObjectName(QString::fromUtf8("timeOffsetDisplay"));
        timeOffsetDisplay->setGeometry(QRect(10, 30, 41, 21));
        timeOffsetDisplay->setFrameShape(QFrame::NoFrame);
        timeOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);
        clockOffsetDisplay = new QLCDNumber(frame);
        clockOffsetDisplay->setObjectName(QString::fromUtf8("clockOffsetDisplay"));
        clockOffsetDisplay->setGeometry(QRect(10, 50, 41, 21));
        clockOffsetDisplay->setFrameShape(QFrame::NoFrame);
        clockOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);
        timeDelayDisplay = new QLCDNumber(frame);
        timeDelayDisplay->setObjectName(QString::fromUtf8("timeDelayDisplay"));
        timeDelayDisplay->setGeometry(QRect(10, 10, 41, 21));
        timeDelayDisplay->setFrameShape(QFrame::NoFrame);
        timeDelayDisplay->setSegmentStyle(QLCDNumber::Flat);
        angleDisplay = new QLCDNumber(frame);
        angleDisplay->setObjectName(QString::fromUtf8("angleDisplay"));
        angleDisplay->setGeometry(QRect(80, 50, 41, 23));
        angleDisplay->setFrameShape(QFrame::NoFrame);
        angleDisplay->setSegmentStyle(QLCDNumber::Flat);
        faadSyncLabel = new QLabel(frame);
        faadSyncLabel->setObjectName(QString::fromUtf8("faadSyncLabel"));
        faadSyncLabel->setGeometry(QRect(390, 70, 31, 21));
        faadSyncLabel->setFrameShape(QFrame::Box);
        messageLabel = new QLabel(frame);
        messageLabel->setObjectName(QString::fromUtf8("messageLabel"));
        messageLabel->setGeometry(QRect(10, 90, 411, 21));
        messageLabel->setFrameShape(QFrame::Box);
        phaseOffsetDisplay = new QLCDNumber(frame);
        phaseOffsetDisplay->setObjectName(QString::fromUtf8("phaseOffsetDisplay"));
        phaseOffsetDisplay->setGeometry(QRect(10, 72, 41, 21));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(8);
        phaseOffsetDisplay->setFont(font);
        phaseOffsetDisplay->setFrameShape(QFrame::NoFrame);
        phaseOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);

        retranslateUi(drmdecoder);

        QMetaObject::connectSlotsByName(drmdecoder);
    } // setupUi

    void retranslateUi(QWidget *drmdecoder)
    {
        drmdecoder->setWindowTitle(QApplication::translate("drmdecoder", "drm decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        show_int_offset->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>&quot;Coarse frequency offset as measured by the software&quot;</p><p><br/></p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        show_small_offset->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Fine frequency offset as measured by the software (i.e. the offset to the nearest carrier).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        timeSyncLabel->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Time sync label, green means time synchronization is OK.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        timeSyncLabel->setText(QString());
        label_2->setText(QApplication::translate("drmdecoder", "time sync", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        facSyncLabel->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>FAC sync label, Green means that FAC's could be decoded (easy, often 4QAM).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        facSyncLabel->setText(QString());
        label_3->setText(QApplication::translate("drmdecoder", "FAC sync", 0, QApplication::UnicodeUTF8));
        modeIndicator->setText(QString());
#ifndef QT_NO_TOOLTIP
        sdcSyncLabel->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>SDC sync label, green means SDC's could be decoded (usually 16 QAM).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        sdcSyncLabel->setText(QString());
        label_4->setText(QApplication::translate("drmdecoder", "SDC sync", 0, QApplication::UnicodeUTF8));
        stationLabel->setText(QString());
        audioModelabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        snrDisplay->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Estimate of the SNR of the DRM signal.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        spectrumIndicator->setText(QString());
#ifndef QT_NO_TOOLTIP
        channel_1->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Visibility of the button suggests the availability of a DRM channel.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        channel_1->setText(QString());
        channel_2->setText(QString());
        channel_3->setText(QString());
        channel_4->setText(QString());
#ifndef QT_NO_TOOLTIP
        timeOffsetDisplay->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Time offset display, i.e. the &quot;fine&quot; time error.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        clockOffsetDisplay->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Estimate of the clock offset (measured between successive frames).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        timeDelayDisplay->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Time delay, i.e. a measure of the timing error.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        angleDisplay->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Phase angle of the error.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        faadSyncLabel->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>Faad sync label. Green means that the AAC decoder could decode audio.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        faadSyncLabel->setText(QString());
        messageLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        phaseOffsetDisplay->setToolTip(QApplication::translate("drmdecoder", "<html><head/><body><p>PhaseOffsetDisplay shows the local offset of the frequency as caused by the mousewheeel</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class drmdecoder: public Ui_drmdecoder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DRMDECODER_H
