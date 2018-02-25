/********************************************************************************
** Form generated from reading UI file 'newradio.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWRADIO_H
#define UI_NEWRADIO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QwtPlot *hfSpectrum;
    QSlider *hfScopeSlider;
    QwtPlot *lfSpectrum;
    QSlider *lfScopeSlider;
    QComboBox *decoderTable;
    QComboBox *deviceTable;
    QLCDNumber *frequencyDisplay;
    QPushButton *freqButton;
    QComboBox *bandSelector;
    QComboBox *streamOutSelector;
    QPushButton *freqSave;
    QPushButton *middleButton;
    QPushButton *quitButton;
    QSpinBox *mouse_Inc;
    QComboBox *AGC_select;
    QSpinBox *agc_thresholdSlider;
    QLabel *timeDisplay;
    QLabel *label_2;
    QPushButton *dumpButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(737, 455);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        hfSpectrum = new QwtPlot(centralwidget);
        hfSpectrum->setObjectName(QString::fromUtf8("hfSpectrum"));
        hfSpectrum->setGeometry(QRect(50, 80, 671, 181));
        hfScopeSlider = new QSlider(centralwidget);
        hfScopeSlider->setObjectName(QString::fromUtf8("hfScopeSlider"));
        hfScopeSlider->setGeometry(QRect(20, 80, 24, 161));
        hfScopeSlider->setOrientation(Qt::Vertical);
        lfSpectrum = new QwtPlot(centralwidget);
        lfSpectrum->setObjectName(QString::fromUtf8("lfSpectrum"));
        lfSpectrum->setGeometry(QRect(170, 260, 551, 171));
        lfScopeSlider = new QSlider(centralwidget);
        lfScopeSlider->setObjectName(QString::fromUtf8("lfScopeSlider"));
        lfScopeSlider->setGeometry(QRect(150, 260, 24, 171));
        lfScopeSlider->setOrientation(Qt::Vertical);
        decoderTable = new QComboBox(centralwidget);
        decoderTable->setObjectName(QString::fromUtf8("decoderTable"));
        decoderTable->setGeometry(QRect(420, 0, 151, 21));
        deviceTable = new QComboBox(centralwidget);
        deviceTable->setObjectName(QString::fromUtf8("deviceTable"));
        deviceTable->setGeometry(QRect(570, 0, 151, 21));
        frequencyDisplay = new QLCDNumber(centralwidget);
        frequencyDisplay->setObjectName(QString::fromUtf8("frequencyDisplay"));
        frequencyDisplay->setGeometry(QRect(90, 0, 211, 61));
        frequencyDisplay->setFrameShape(QFrame::NoFrame);
        frequencyDisplay->setDigitCount(10);
        frequencyDisplay->setSegmentStyle(QLCDNumber::Flat);
        freqButton = new QPushButton(centralwidget);
        freqButton->setObjectName(QString::fromUtf8("freqButton"));
        freqButton->setGeometry(QRect(10, 260, 121, 51));
        bandSelector = new QComboBox(centralwidget);
        bandSelector->setObjectName(QString::fromUtf8("bandSelector"));
        bandSelector->setGeometry(QRect(10, 370, 121, 21));
        streamOutSelector = new QComboBox(centralwidget);
        streamOutSelector->setObjectName(QString::fromUtf8("streamOutSelector"));
        streamOutSelector->setGeometry(QRect(420, 20, 151, 41));
        freqSave = new QPushButton(centralwidget);
        freqSave->setObjectName(QString::fromUtf8("freqSave"));
        freqSave->setGeometry(QRect(10, 340, 121, 31));
        middleButton = new QPushButton(centralwidget);
        middleButton->setObjectName(QString::fromUtf8("middleButton"));
        middleButton->setGeometry(QRect(10, 310, 121, 31));
        quitButton = new QPushButton(centralwidget);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));
        quitButton->setGeometry(QRect(570, 20, 151, 41));
        mouse_Inc = new QSpinBox(centralwidget);
        mouse_Inc->setObjectName(QString::fromUtf8("mouse_Inc"));
        mouse_Inc->setGeometry(QRect(10, 30, 96, 21));
        mouse_Inc->setValue(5);
        AGC_select = new QComboBox(centralwidget);
        AGC_select->setObjectName(QString::fromUtf8("AGC_select"));
        AGC_select->setGeometry(QRect(320, 0, 101, 41));
        agc_thresholdSlider = new QSpinBox(centralwidget);
        agc_thresholdSlider->setObjectName(QString::fromUtf8("agc_thresholdSlider"));
        agc_thresholdSlider->setGeometry(QRect(320, 40, 101, 21));
        agc_thresholdSlider->setMinimum(-144);
        agc_thresholdSlider->setMaximum(0);
        agc_thresholdSlider->setValue(-100);
        timeDisplay = new QLabel(centralwidget);
        timeDisplay->setObjectName(QString::fromUtf8("timeDisplay"));
        timeDisplay->setGeometry(QRect(160, 50, 141, 16));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 10, 21, 21));
        QFont font;
        font.setPointSize(14);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);
        dumpButton = new QPushButton(centralwidget);
        dumpButton->setObjectName(QString::fromUtf8("dumpButton"));
        dumpButton->setGeometry(QRect(10, 390, 121, 41));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "sdr-j-radio", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        hfSpectrum->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Main Spectrum Window. This window shows the spectrum of the signal arriving from the selected input device, decimated to the selected inputrate.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        decoderTable->clear();
        decoderTable->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "select decoder", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        decoderTable->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>decoder selector. The decoders listed here are part of the configured implementation.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deviceTable->clear();
        deviceTable->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "select device", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        deviceTable->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>device selector. Select an input device and start running.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        freqButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Touching this button will cause a widget to appear with a keypad on which a frequency - in kHz or mHz - can be specified. Use the kHz or mHz button on this pad to acknowledge the choice.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        freqButton->setText(QApplication::translate("MainWindow", "select freq", 0, QApplication::UnicodeUTF8));
        bandSelector->clear();
        bandSelector->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "am", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "usb", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "lsb", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "wide", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        bandSelector->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>band selector. One may choose among several precoded bandwidth settings.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        streamOutSelector->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>select an audio output device. </p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        freqSave->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>The selected frequency can be saved. Touching this button will show a small widget where you</p><p>can give the frequency a name as label.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        freqSave->setText(QApplication::translate("MainWindow", "frequency save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        middleButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Touching this button will change the oscillator such that the selected frequency will be in the middke</p><p>of the right half of the frequency display.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        middleButton->setText(QApplication::translate("MainWindow", "middle", 0, QApplication::UnicodeUTF8));
        quitButton->setText(QApplication::translate("MainWindow", "QUIT", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        mouse_Inc->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Mouse Increment. The amount of Hz that will change when moving the mousewheel is set here.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        AGC_select->clear();
        AGC_select->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "AGC off", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "slow", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "fast", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        AGC_select->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Agc, automatic gain control, will be enabled when the agc is switched on and the signal strength exceeds</p><p>the value in the spinbox. </p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        timeDisplay->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_2->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>SWRadio copyright:</p><p>Copyright of the Qt toolkit used: the Qt Company</p><p>Copyright of the libraries used for SDRplay, portaudio, libsndfile and libsamplerate to their developers</p><p>Copyright of the Reed Solomon Decoder software: Phil Karns</p><p>All copyrights gratefully acknowledged</p><p>CopyRight: 2018, Jan van Katwijk, Lazy Chair Computing.</p><p>sdr-j swradio (an SDR-J program) is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.</p><p><br/></p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "\302\251", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        dumpButton->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>The incoming data stream can be written to a regular pcm (i.e. .wav) file. Touching this button will</p><p>show a menu for selecting a filename, after selecting the filename writing will start (the text on the</p><p>button will show &quot;writing&quot;). Touching the button again will close the file.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        dumpButton->setText(QApplication::translate("MainWindow", "dump", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWRADIO_H
