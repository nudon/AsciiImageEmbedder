/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "imagebuilder.h"
#include "imageviewer.h"
#include "optionsetter.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *options;
    QWidget *input;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    optionSetter *lineEdit_2;
    QWidget *output;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_6;
    optionSetter *lineEdit_3;
    QWidget *font;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_10;
    optionSetter *lineEdit_7;
    QWidget *fontsize;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_11;
    optionSetter *lineEdit_8;
    QWidget *xspacing;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label12;
    optionSetter *lineEdit_9;
    QWidget *yspacing_2;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_13;
    optionSetter *lineEdit_10;
    QWidget *ascii;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_14;
    optionSetter *lineEdit_11;
    QWidget *hiragana;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_15;
    optionSetter *lineEdit_12;
    QWidget *katakana;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_16;
    optionSetter *lineEdit_13;
    QWidget *edge;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_17;
    optionSetter *lineEdit_14;
    QWidget *color;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_18;
    optionSetter *lineEdit_15;
    QWidget *hue;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_20;
    optionSetter *lineEdit_17;
    QWidget *saturation;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_19;
    optionSetter *lineEdit_16;
    QWidget *lightness;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_21;
    optionSetter *lineEdit_18;
    imageBuilder *pushButton;
    QVBoxLayout *images;
    QLabel *label;
    imageViewer *basePicture;
    QLabel *asciiPicture;
    imageViewer *textPicture;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(711, 708);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(9);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        options = new QVBoxLayout();
        options->setSpacing(0);
        options->setObjectName(QStringLiteral("options"));
        input = new QWidget(centralWidget);
        input->setObjectName(QStringLiteral("input"));
        horizontalLayout_2 = new QHBoxLayout(input);
        horizontalLayout_2->setSpacing(4);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, 0);
        label_4 = new QLabel(input);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);

        lineEdit_2 = new optionSetter(input);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        horizontalLayout_2->addWidget(lineEdit_2);


        options->addWidget(input);

        output = new QWidget(centralWidget);
        output->setObjectName(QStringLiteral("output"));
        horizontalLayout_3 = new QHBoxLayout(output);
        horizontalLayout_3->setSpacing(4);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 0, -1, 0);
        label_6 = new QLabel(output);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_3->addWidget(label_6);

        lineEdit_3 = new optionSetter(output);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));

        horizontalLayout_3->addWidget(lineEdit_3);


        options->addWidget(output);

        font = new QWidget(centralWidget);
        font->setObjectName(QStringLiteral("font"));
        horizontalLayout_4 = new QHBoxLayout(font);
        horizontalLayout_4->setSpacing(4);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, 0);
        label_10 = new QLabel(font);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_4->addWidget(label_10);

        lineEdit_7 = new optionSetter(font);
        lineEdit_7->setObjectName(QStringLiteral("lineEdit_7"));

        horizontalLayout_4->addWidget(lineEdit_7);


        options->addWidget(font);

        fontsize = new QWidget(centralWidget);
        fontsize->setObjectName(QStringLiteral("fontsize"));
        horizontalLayout_5 = new QHBoxLayout(fontsize);
        horizontalLayout_5->setSpacing(4);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, 0, -1, 0);
        label_11 = new QLabel(fontsize);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_5->addWidget(label_11);

        lineEdit_8 = new optionSetter(fontsize);
        lineEdit_8->setObjectName(QStringLiteral("lineEdit_8"));

        horizontalLayout_5->addWidget(lineEdit_8);


        options->addWidget(fontsize);

        xspacing = new QWidget(centralWidget);
        xspacing->setObjectName(QStringLiteral("xspacing"));
        horizontalLayout_6 = new QHBoxLayout(xspacing);
        horizontalLayout_6->setSpacing(4);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, 0, -1, 0);
        label12 = new QLabel(xspacing);
        label12->setObjectName(QStringLiteral("label12"));

        horizontalLayout_6->addWidget(label12);

        lineEdit_9 = new optionSetter(xspacing);
        lineEdit_9->setObjectName(QStringLiteral("lineEdit_9"));

        horizontalLayout_6->addWidget(lineEdit_9);


        options->addWidget(xspacing);

        yspacing_2 = new QWidget(centralWidget);
        yspacing_2->setObjectName(QStringLiteral("yspacing_2"));
        horizontalLayout_7 = new QHBoxLayout(yspacing_2);
        horizontalLayout_7->setSpacing(4);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(-1, 0, -1, 0);
        label_13 = new QLabel(yspacing_2);
        label_13->setObjectName(QStringLiteral("label_13"));

        horizontalLayout_7->addWidget(label_13);

        lineEdit_10 = new optionSetter(yspacing_2);
        lineEdit_10->setObjectName(QStringLiteral("lineEdit_10"));

        horizontalLayout_7->addWidget(lineEdit_10);


        options->addWidget(yspacing_2);

        ascii = new QWidget(centralWidget);
        ascii->setObjectName(QStringLiteral("ascii"));
        horizontalLayout_8 = new QHBoxLayout(ascii);
        horizontalLayout_8->setSpacing(4);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(-1, 0, -1, 0);
        label_14 = new QLabel(ascii);
        label_14->setObjectName(QStringLiteral("label_14"));

        horizontalLayout_8->addWidget(label_14);

        lineEdit_11 = new optionSetter(ascii);
        lineEdit_11->setObjectName(QStringLiteral("lineEdit_11"));

        horizontalLayout_8->addWidget(lineEdit_11);


        options->addWidget(ascii);

        hiragana = new QWidget(centralWidget);
        hiragana->setObjectName(QStringLiteral("hiragana"));
        horizontalLayout_9 = new QHBoxLayout(hiragana);
        horizontalLayout_9->setSpacing(4);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(-1, 0, -1, 0);
        label_15 = new QLabel(hiragana);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_9->addWidget(label_15);

        lineEdit_12 = new optionSetter(hiragana);
        lineEdit_12->setObjectName(QStringLiteral("lineEdit_12"));

        horizontalLayout_9->addWidget(lineEdit_12);


        options->addWidget(hiragana);

        katakana = new QWidget(centralWidget);
        katakana->setObjectName(QStringLiteral("katakana"));
        horizontalLayout_10 = new QHBoxLayout(katakana);
        horizontalLayout_10->setSpacing(4);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(-1, 0, -1, 0);
        label_16 = new QLabel(katakana);
        label_16->setObjectName(QStringLiteral("label_16"));

        horizontalLayout_10->addWidget(label_16);

        lineEdit_13 = new optionSetter(katakana);
        lineEdit_13->setObjectName(QStringLiteral("lineEdit_13"));

        horizontalLayout_10->addWidget(lineEdit_13);


        options->addWidget(katakana);

        edge = new QWidget(centralWidget);
        edge->setObjectName(QStringLiteral("edge"));
        horizontalLayout_11 = new QHBoxLayout(edge);
        horizontalLayout_11->setSpacing(4);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(-1, 0, -1, 0);
        label_17 = new QLabel(edge);
        label_17->setObjectName(QStringLiteral("label_17"));

        horizontalLayout_11->addWidget(label_17);

        lineEdit_14 = new optionSetter(edge);
        lineEdit_14->setObjectName(QStringLiteral("lineEdit_14"));

        horizontalLayout_11->addWidget(lineEdit_14);


        options->addWidget(edge);

        color = new QWidget(centralWidget);
        color->setObjectName(QStringLiteral("color"));
        horizontalLayout_12 = new QHBoxLayout(color);
        horizontalLayout_12->setSpacing(4);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalLayout_12->setContentsMargins(-1, 0, -1, 0);
        label_18 = new QLabel(color);
        label_18->setObjectName(QStringLiteral("label_18"));

        horizontalLayout_12->addWidget(label_18);

        lineEdit_15 = new optionSetter(color);
        lineEdit_15->setObjectName(QStringLiteral("lineEdit_15"));

        horizontalLayout_12->addWidget(lineEdit_15);


        options->addWidget(color);

        hue = new QWidget(centralWidget);
        hue->setObjectName(QStringLiteral("hue"));
        horizontalLayout_14 = new QHBoxLayout(hue);
        horizontalLayout_14->setSpacing(4);
        horizontalLayout_14->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        horizontalLayout_14->setContentsMargins(-1, 0, -1, 0);
        label_20 = new QLabel(hue);
        label_20->setObjectName(QStringLiteral("label_20"));

        horizontalLayout_14->addWidget(label_20);

        lineEdit_17 = new optionSetter(hue);
        lineEdit_17->setObjectName(QStringLiteral("lineEdit_17"));

        horizontalLayout_14->addWidget(lineEdit_17);


        options->addWidget(hue);

        saturation = new QWidget(centralWidget);
        saturation->setObjectName(QStringLiteral("saturation"));
        horizontalLayout_13 = new QHBoxLayout(saturation);
        horizontalLayout_13->setSpacing(4);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(-1, 0, -1, 0);
        label_19 = new QLabel(saturation);
        label_19->setObjectName(QStringLiteral("label_19"));

        horizontalLayout_13->addWidget(label_19);

        lineEdit_16 = new optionSetter(saturation);
        lineEdit_16->setObjectName(QStringLiteral("lineEdit_16"));

        horizontalLayout_13->addWidget(lineEdit_16);


        options->addWidget(saturation);

        lightness = new QWidget(centralWidget);
        lightness->setObjectName(QStringLiteral("lightness"));
        horizontalLayout_15 = new QHBoxLayout(lightness);
        horizontalLayout_15->setSpacing(4);
        horizontalLayout_15->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(-1, 0, -1, 0);
        label_21 = new QLabel(lightness);
        label_21->setObjectName(QStringLiteral("label_21"));

        horizontalLayout_15->addWidget(label_21);

        lineEdit_18 = new optionSetter(lightness);
        lineEdit_18->setObjectName(QStringLiteral("lineEdit_18"));

        horizontalLayout_15->addWidget(lineEdit_18);


        options->addWidget(lightness);

        pushButton = new imageBuilder(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        options->addWidget(pushButton);


        horizontalLayout->addLayout(options);

        images = new QVBoxLayout();
        images->setSpacing(6);
        images->setObjectName(QStringLiteral("images"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        images->addWidget(label);

        basePicture = new imageViewer(centralWidget);
        basePicture->setObjectName(QStringLiteral("basePicture"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(basePicture->sizePolicy().hasHeightForWidth());
        basePicture->setSizePolicy(sizePolicy);
        basePicture->setMinimumSize(QSize(0, 0));
        basePicture->setFrameShape(QFrame::StyledPanel);
        basePicture->setScaledContents(false);
        basePicture->setAlignment(Qt::AlignCenter);

        images->addWidget(basePicture);

        asciiPicture = new QLabel(centralWidget);
        asciiPicture->setObjectName(QStringLiteral("asciiPicture"));

        images->addWidget(asciiPicture);

        textPicture = new imageViewer(centralWidget);
        textPicture->setObjectName(QStringLiteral("textPicture"));
        sizePolicy.setHeightForWidth(textPicture->sizePolicy().hasHeightForWidth());
        textPicture->setSizePolicy(sizePolicy);
        textPicture->setFrameShape(QFrame::StyledPanel);
        textPicture->setScaledContents(false);
        textPicture->setAlignment(Qt::AlignCenter);

        images->addWidget(textPicture);

        images->setStretch(0, 10);
        images->setStretch(1, 40);
        images->setStretch(2, 10);
        images->setStretch(3, 40);

        horizontalLayout->addLayout(images);

        horizontalLayout->setStretch(0, 40);
        horizontalLayout->setStretch(1, 60);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 711, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(pushButton, SIGNAL(buildComplete(char*)), textPicture, SLOT(showNewImage(char*)));
        QObject::connect(lineEdit_3, SIGNAL(returnPressed()), lineEdit_3, SLOT(gui_setOutputFile()));
        QObject::connect(lineEdit_7, SIGNAL(returnPressed()), lineEdit_7, SLOT(gui_setFont()));
        QObject::connect(lineEdit_8, SIGNAL(returnPressed()), lineEdit_8, SLOT(gui_setFontSize()));
        QObject::connect(lineEdit_9, SIGNAL(returnPressed()), lineEdit_9, SLOT(gui_setSpaceX()));
        QObject::connect(lineEdit_10, SIGNAL(returnPressed()), lineEdit_10, SLOT(gui_setSpaceY()));
        QObject::connect(lineEdit_11, SIGNAL(returnPressed()), lineEdit_11, SLOT(gui_setAsciiUsed()));
        QObject::connect(lineEdit_12, SIGNAL(returnPressed()), lineEdit_12, SLOT(gui_setHiraganaUsed()));
        QObject::connect(lineEdit_13, SIGNAL(returnPressed()), lineEdit_13, SLOT(gui_setKatakanaUsed()));
        QObject::connect(lineEdit_14, SIGNAL(returnPressed()), lineEdit_14, SLOT(gui_setEdgeScoreWeight()));
        QObject::connect(lineEdit_16, SIGNAL(returnPressed()), lineEdit_16, SLOT(gui_setSaturationScale()));
        QObject::connect(lineEdit_17, SIGNAL(returnPressed()), lineEdit_17, SLOT(gui_setHueScale()));
        QObject::connect(lineEdit_18, SIGNAL(returnPressed()), lineEdit_18, SLOT(gui_setLightnessScale()));
        QObject::connect(lineEdit_2, SIGNAL(returnPressed()), lineEdit_2, SLOT(gui_setInputFile()));
        QObject::connect(lineEdit_2, SIGNAL(input_file_changed(char*)), basePicture, SLOT(showNewImage(char*)));
        QObject::connect(pushButton, SIGNAL(released()), pushButton, SLOT(buildAsciiImage()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Input File", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Output File", nullptr));
        lineEdit_3->setText(QApplication::translate("MainWindow", "output.png", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "Set Font", nullptr));
        lineEdit_7->setText(QApplication::translate("MainWindow", "comicSans.ttf", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "Set Font Size", nullptr));
        lineEdit_8->setText(QApplication::translate("MainWindow", "10", nullptr));
        label12->setText(QApplication::translate("MainWindow", "Set x spacing", nullptr));
        lineEdit_9->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "Set y spacing", nullptr));
        lineEdit_10->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "Set Ascii Used", nullptr));
        lineEdit_11->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_15->setText(QApplication::translate("MainWindow", "Set Hiragana Used", nullptr));
        lineEdit_12->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_16->setText(QApplication::translate("MainWindow", "Set Katakana Used", nullptr));
        lineEdit_13->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_17->setText(QApplication::translate("MainWindow", "Set Edge Score Weigt", nullptr));
        lineEdit_14->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_18->setText(QApplication::translate("MainWindow", "Set Color Score Weight", nullptr));
        lineEdit_15->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_20->setText(QApplication::translate("MainWindow", "Set Hue Weight", nullptr));
        lineEdit_17->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_19->setText(QApplication::translate("MainWindow", "Set Saturation Weight", nullptr));
        lineEdit_16->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_21->setText(QApplication::translate("MainWindow", "Set Lightness Weight", nullptr));
        lineEdit_18->setText(QApplication::translate("MainWindow", "1", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Generate Image", nullptr));
        label->setText(QApplication::translate("MainWindow", "Base Image", nullptr));
        basePicture->setText(QString());
        asciiPicture->setText(QApplication::translate("MainWindow", "Ascii Image", nullptr));
        textPicture->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
