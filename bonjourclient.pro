#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T13:39:50
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bonjourclient
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


SOURCES       = client.cpp \
                main.cpp \
                bonjourservicebrowser.cpp \
                bonjourserviceresolver.cpp \
                utility.cpp

HEADERS       = client.h \
                bonjourservicebrowser.h \
                bonjourserviceresolver.h \
                utility.h \
    peerinfo.h

!mac:x11:LIBS+=-ldns_sd

win32:LIBS+=-L"C:/Program Files/Bonjour SDK/Lib/Win32" -ldnssd

INCLUDEPATH += "C:/Program Files/Bonjour SDK/Include"

HEADERS += dns_sd.h
