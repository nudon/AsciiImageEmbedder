#-------------------------------------------------
#
# Project created by QtCreator 2018-09-26T23:25:15
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = QtAskii
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
LIBS += `pkg-config --cflags --libs MagickWand freetype2`


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        optionsetter.cpp \
        imagegenerator.cpp \
    imagebuilder.cpp \
    imageviewer.cpp


HEADERS += \
        mainwindow.h \
        optionsetter.h \
        #$$PWD/../characters.h \
        #$$PWD/../region.h \
        #$$PWD/../picture.h \
    imagegenerator.h \
    imagebuilder.h \
    imageviewer.h

OBJECTS += $$PWD/../chars.o \
           $$PWD/../region.o \
           $$PWD/../picture.o \
           $$PWD/../dataStructures.o \
           $$PWD/../myList.o

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
