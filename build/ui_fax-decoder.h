/********************************************************************************
** Form generated from reading UI file 'fax-decoder.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FAX_2D_DECODER_H
#define UI_FAX_2D_DECODER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fax_decoder
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *showState;
    QLCDNumber *lineNumber;
    QLCDNumber *aptFreq;
    QLabel *workText;
    QComboBox *colorSetter;
    QPushButton *skipSetter;
    QComboBox *iocSetter;
    QComboBox *modeSetter;
    QSpinBox *startSetter;
    QSpinBox *stopSetter;
    QSpinBox *carrierSetter;
    QSpinBox *deviationSetter;
    QLabel *slantText;
    QSpinBox *lpmSetter;
    QComboBox *phaseSetter;
    QPushButton *resetButton;
    QPushButton *saveButton;
    QPushButton *autoContinueButton;

    void setupUi(QWidget *fax_decoder)
    {
        if (fax_decoder->objectName().isEmpty())
            fax_decoder->setObjectName(QString::fromUtf8("fax_decoder"));
        fax_decoder->resize(574, 155);
        frame = new QFrame(fax_decoder);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 571, 141));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(440, 110, 121, 21));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);
        showState = new QLabel(frame);
        showState->setObjectName(QString::fromUtf8("showState"));
        showState->setGeometry(QRect(450, 40, 101, 31));
        showState->setFrameShape(QFrame::Box);
        lineNumber = new QLCDNumber(frame);
        lineNumber->setObjectName(QString::fromUtf8("lineNumber"));
        lineNumber->setGeometry(QRect(490, 80, 64, 23));
        lineNumber->setLineWidth(0);
        lineNumber->setSegmentStyle(QLCDNumber::Flat);
        aptFreq = new QLCDNumber(frame);
        aptFreq->setObjectName(QString::fromUtf8("aptFreq"));
        aptFreq->setGeometry(QRect(410, 80, 64, 23));
        aptFreq->setLineWidth(0);
        aptFreq->setSegmentStyle(QLCDNumber::Flat);
        workText = new QLabel(frame);
        workText->setObjectName(QString::fromUtf8("workText"));
        workText->setGeometry(QRect(270, 110, 151, 20));
        colorSetter = new QComboBox(frame);
        colorSetter->setObjectName(QString::fromUtf8("colorSetter"));
        colorSetter->setGeometry(QRect(340, 10, 101, 29));
        skipSetter = new QPushButton(frame);
        skipSetter->setObjectName(QString::fromUtf8("skipSetter"));
        skipSetter->setGeometry(QRect(450, 10, 101, 31));
        iocSetter = new QComboBox(frame);
        iocSetter->setObjectName(QString::fromUtf8("iocSetter"));
        iocSetter->setGeometry(QRect(10, 90, 121, 29));
        modeSetter = new QComboBox(frame);
        modeSetter->setObjectName(QString::fromUtf8("modeSetter"));
        modeSetter->setGeometry(QRect(160, 70, 141, 41));
        startSetter = new QSpinBox(frame);
        startSetter->setObjectName(QString::fromUtf8("startSetter"));
        startSetter->setGeometry(QRect(10, 10, 91, 21));
        startSetter->setMaximum(1000);
        startSetter->setValue(300);
        stopSetter = new QSpinBox(frame);
        stopSetter->setObjectName(QString::fromUtf8("stopSetter"));
        stopSetter->setGeometry(QRect(10, 30, 91, 21));
        stopSetter->setMaximum(1000);
        stopSetter->setValue(450);
        carrierSetter = new QSpinBox(frame);
        carrierSetter->setObjectName(QString::fromUtf8("carrierSetter"));
        carrierSetter->setGeometry(QRect(120, 10, 121, 21));
        carrierSetter->setMaximum(2500);
        carrierSetter->setValue(1000);
        deviationSetter = new QSpinBox(frame);
        deviationSetter->setObjectName(QString::fromUtf8("deviationSetter"));
        deviationSetter->setGeometry(QRect(10, 50, 91, 21));
        deviationSetter->setMaximum(1000);
        deviationSetter->setValue(400);
        slantText = new QLabel(frame);
        slantText->setObjectName(QString::fromUtf8("slantText"));
        slantText->setGeometry(QRect(460, 130, 151, 21));
        lpmSetter = new QSpinBox(frame);
        lpmSetter->setObjectName(QString::fromUtf8("lpmSetter"));
        lpmSetter->setGeometry(QRect(10, 70, 91, 21));
        lpmSetter->setMaximum(200);
        lpmSetter->setValue(120);
        phaseSetter = new QComboBox(frame);
        phaseSetter->setObjectName(QString::fromUtf8("phaseSetter"));
        phaseSetter->setGeometry(QRect(240, 10, 101, 29));
        resetButton = new QPushButton(frame);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setGeometry(QRect(300, 70, 97, 41));
        saveButton = new QPushButton(frame);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setGeometry(QRect(160, 40, 131, 31));
        autoContinueButton = new QPushButton(frame);
        autoContinueButton->setObjectName(QString::fromUtf8("autoContinueButton"));
        autoContinueButton->setGeometry(QRect(290, 40, 151, 31));

        retranslateUi(fax_decoder);

        QMetaObject::connectSlotsByName(fax_decoder);
    } // setupUi

    void retranslateUi(QWidget *fax_decoder)
    {
        fax_decoder->setWindowTitle(QApplication::translate("fax_decoder", "fax-decoder", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("fax_decoder", "fax decoder", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        showState->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Current state of operation.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        showState->setText(QString());
        workText->setText(QString());
        colorSetter->clear();
        colorSetter->insertItems(0, QStringList()
         << QApplication::translate("fax_decoder", "BW", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "GRAY", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "COLOR", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        colorSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Black and white or color.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        skipSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Skip the current operation state and go the next one. Comes in handy when there is signal, but beyond the initial synchronization.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        skipSetter->setText(QApplication::translate("fax_decoder", "skip Mode", 0, QApplication::UnicodeUTF8));
        iocSetter->clear();
        iocSetter->insertItems(0, QStringList()
         << QApplication::translate("fax_decoder", "Wefax576", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "Wefax288", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "HamColor", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "Ham288b", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "Color240", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "FAX480", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        iocSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>FAX mode, implies some settings.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        modeSetter->clear();
        modeSetter->insertItems(0, QStringList()
         << QApplication::translate("fax_decoder", "FM", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "AM", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        startSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Start frequency, normally 300.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        stopSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Stop frequency of a transmission. Normally 450.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        carrierSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Offset of the central carrier of the signal. In mechanical FAX machines this is normally 1900, we choose for a smaller value.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        deviationSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Deviation of the central signal frequency.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        slantText->setText(QString());
#ifndef QT_NO_TOOLTIP
        lpmSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Lines per minute.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        phaseSetter->clear();
        phaseSetter->insertItems(0, QStringList()
         << QApplication::translate("fax_decoder", "phase", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("fax_decoder", "inverse", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        phaseSetter->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Normal or inverse (i.e.back and white).</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetButton->setText(QApplication::translate("fax_decoder", "reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        saveButton->setToolTip(QApplication::translate("fax_decoder", "<html><head/><body><p>Save the picture when the end is reached. A menu will apear at the end with which a file(name) can be selected.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveButton->setText(QApplication::translate("fax_decoder", "save on Done", 0, QApplication::UnicodeUTF8));
        autoContinueButton->setText(QApplication::translate("fax_decoder", "continue", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class fax_decoder: public Ui_fax_decoder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FAX_2D_DECODER_H
