/********************************************************************************
** Form generated from reading UI file 'filereader-widget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEREADER_2D_WIDGET_H
#define UI_FILEREADER_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_filereader
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *nameofFile;
    QPushButton *resetButton;
    QScrollBar *progressBar;
    QLCDNumber *rateDisplay;

    void setupUi(QWidget *filereader)
    {
        if (filereader->objectName().isEmpty())
            filereader->setObjectName(QString::fromUtf8("filereader"));
        filereader->resize(323, 243);
        frame = new QFrame(filereader);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 311, 231));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(60, 10, 91, 21));
        nameofFile = new QLabel(frame);
        nameofFile->setObjectName(QString::fromUtf8("nameofFile"));
        nameofFile->setGeometry(QRect(10, 50, 271, 21));
        resetButton = new QPushButton(frame);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setGeometry(QRect(20, 160, 97, 31));
        progressBar = new QScrollBar(frame);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(0, 140, 291, 20));
        progressBar->setMaximum(100);
        progressBar->setOrientation(Qt::Horizontal);
        rateDisplay = new QLCDNumber(frame);
        rateDisplay->setObjectName(QString::fromUtf8("rateDisplay"));
        rateDisplay->setGeometry(QRect(160, 90, 121, 23));
        rateDisplay->setFrameShape(QFrame::NoFrame);
        rateDisplay->setDigitCount(7);
        rateDisplay->setSegmentStyle(QLCDNumber::Flat);

        retranslateUi(filereader);

        QMetaObject::connectSlotsByName(filereader);
    } // setupUi

    void retranslateUi(QWidget *filereader)
    {
        filereader->setWindowTitle(QApplication::translate("filereader", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("filereader", "fileReader", 0, QApplication::UnicodeUTF8));
        nameofFile->setText(QString());
        resetButton->setText(QApplication::translate("filereader", "reset", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class filereader: public Ui_filereader {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEREADER_2D_WIDGET_H
