#-------------------------------------------------
#
# Project created by QtCreator 2014-05-21T21:58:12
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CogView
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    wiithread.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    wiithread.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/wiic

LIBS += -L/usr/local/lib -lwiicpp
