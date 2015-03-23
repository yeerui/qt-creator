#-------------------------------------------------
#
# Project created by QtCreator 2015-03-12T12:50:40
#
#-------------------------------------------------

QT       += core network testlib

QT       -= gui

TARGET = codemodelbackendtest
CONFIG   += console c++14 testcase
CONFIG   -= app_bundle

TEMPLATE = app

GMOCK_DIR = $$(GMOCK_DIR)
GTEST_DIR = $$GMOCK_DIR/gtest

requires(exists($$GMOCK_DIR))
!exists($$GMOCK_DIR):message("No gmock is found! To enabe unit tests set GMOCK_DIR")

INCLUDEPATH += $$GTEST_DIR $$GTEST_DIR/include $$GMOCK_DIR $$GMOCK_DIR/include

include(../../../../src/libs/sqlite/sqlite-lib.pri)
include(../../../../src/libs/codemodelbackendipc/codemodelbackendipc-lib.pri)
include(../../../../src/tools/codemodelbackend/ipcsource/codemodelbackendclangipc-source.pri)

SOURCES += main.cpp \
    $$GTEST_DIR/src/gtest-all.cc \
    $$GMOCK_DIR/src/gmock-all.cc \
    utf8test.cpp \
    sqlstatementbuildertest.cpp \
    createtablesqlstatementbuildertest.cpp \
    sqlitecolumntest.cpp \
    sqlitestatementtest.cpp \
    sqlitetabletest.cpp \
    spydummy.cpp \
    sqlitedatabasetest.cpp \
    sqlitedatabasebackendtest.cpp \
    readandwritecommandblocktest.cpp \
    clientserverinprocesstest.cpp \
    clientserveroutsideprocess.cpp \
    gtest-qt-printing.cpp \
    clanginprocesstest.cpp

HEADERS += \
    gtest-qt-printing.h \
    spydummy.h

DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += CODEMODELBACKENDIPC_TESTS
DEFINES += DONT_CHECK_COMMAND_COUNTER
