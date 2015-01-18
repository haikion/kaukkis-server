#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T03:37:12
#
#-------------------------------------------------

QT       += core network gui

CONFIG += C++11
LIBS += -lXtst -lX11

TARGET = kaukkis-server
INCLUDEPATH += src
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += src/main.cpp \
    src/server.cpp \
    src/message.cpp

HEADERS += \
    src/server.h \
    src/message.h

DISTFILES +=
