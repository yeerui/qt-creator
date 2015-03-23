#-------------------------------------------------
#
# Project created by QtCreator 2014-12-17T14:40:23
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = codemodelbackendserver
CONFIG   += console c++14
CONFIG   -= app_bundle

TEMPLATE = app

unix:LIBS += -ldl

include(../../../../src/libs/codemodelbackendipc/codemodelbackendipc-lib.pri)

SOURCES += \
    codemodelbackendserverprocessmain.cpp \
    echoipcserver.cpp

HEADERS += \
    echoipcserver.h

DEFINES += CODEMODELBACKENDIPC_TESTS
DEFINES += DONT_CHECK_COMMAND_COUNTER
