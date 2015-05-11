QT       += core network

QT       -= gui

TARGET = echoserver
CONFIG   += console c++14
CONFIG   -= app_bundle

TEMPLATE = app

unix:LIBS += -ldl

include(../../../../src/libs/codemodelbackendipc/codemodelbackendipc-lib.pri)
include(../../../../src/libs/sqlite/sqlite-lib.pri)

SOURCES += \
    echoipcserver.cpp \
    echoserverprocessmain.cpp

HEADERS += \
    echoipcserver.h

DEFINES += CODEMODELBACKENDIPC_TESTS
DEFINES += DONT_CHECK_COMMAND_COUNTER
