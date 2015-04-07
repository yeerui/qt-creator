VPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += clangipcserver.h \
           codecompleter.h \
    $$PWD/clangstring.h \
    $$PWD/translationunit.h

SOURCES += clangipcserver.cpp \
           codecompleter.cpp \
    $$PWD/clangstring.cpp \
    $$PWD/translationunit.cpp
