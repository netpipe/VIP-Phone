#-------------------------------------------------
#
# Project created by QtCreator 2013-10-16T23:39:04
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++17 -funroll-all-loops -O3

TARGET = Steganograf
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filechooser.cpp

HEADERS  += mainwindow.h \
    filechooser.h

FORMS    += mainwindow.ui \
    filechooser.ui

RESOURCES += \
    res.qrc
