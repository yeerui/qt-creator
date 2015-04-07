VPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += clangipcserver.h \
           codecompleter.h \
    $$PWD/clangstring.h \
    $$PWD/translationunit.h \
    $$PWD/translationunitisnullexception.h \
    $$PWD/translationunitfilenotexits.h \
    $$PWD/codecompletefailedexception.h \
    $$PWD/clangcodecompleteresults.h

SOURCES += clangipcserver.cpp \
           codecompleter.cpp \
    $$PWD/clangstring.cpp \
    $$PWD/translationunit.cpp \
    $$PWD/translationunitisnullexception.cpp \
    $$PWD/translationunitfilenotexits.cpp \
    $$PWD/codecompletefailedexception.cpp \
    $$PWD/clangcodecompleteresults.cpp
