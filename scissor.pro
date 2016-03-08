#-------------------------------------------------
#
# Project created by QtCreator 2016-02-28T19:46:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scissor
TEMPLATE = app


SOURCES += main.cpp\
        scissor.cpp

HEADERS  += scissor.h

FORMS    += scissor.ui

INCLUDEPATH += /usr/include/opencv

INCLUDEPATH += /usr/include/opencv2


LIBS +=  -lopencv_core -lopencv_highgui -lopencv_imgproc
