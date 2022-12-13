#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T14:41:59
#
#-------------------------------------------------

QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VIPPhone
TEMPLATE = app

linux:LIBS += -lSoundTouch
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS soundtouch2 FTP

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    ftp-server/dataconnection.cpp \
    ftp-server/debuglogdialog.cpp \
    ftp-server/ftpcommand.cpp \
    ftp-server/ftpcontrolconnection.cpp \
    ftp-server/ftpgui.cpp \
    ftp-server/ftplistcommand.cpp \
    ftp-server/ftpretrcommand.cpp \
    ftp-server/ftpserver.cpp \
    ftp-server/ftpstorcommand.cpp \
    ftp-server/sslserver.cpp \
        mainwindow.cpp \
    voiceio.cpp \
    voicesocket.cpp \
    buffer.cpp \
    messenger.cpp

HEADERS  += mainwindow.h \
    ftp-server/dataconnection.h \
    ftp-server/debuglogdialog.h \
    ftp-server/ftpcommand.h \
    ftp-server/ftpcontrolconnection.h \
    ftp-server/ftpgui.h \
    ftp-server/ftplistcommand.h \
    ftp-server/ftpretrcommand.h \
    ftp-server/ftpserver.h \
    ftp-server/ftpstorcommand.h \
    ftp-server/sslserver.h \
    voiceio.h \
    voicesocket.h \
    buffer.h \
    messenger.h

FORMS    += mainwindow.ui \
    ftp-server/debuglogdialog.ui \
    ftp-server/ftpgui.ui
