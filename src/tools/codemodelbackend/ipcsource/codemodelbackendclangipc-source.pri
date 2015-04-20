VPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += clangipcserver.h \
           codecompleter.h \
    $$PWD/clangstring.h \
    $$PWD/translationunit.h \
    $$PWD/translationunitisnullexception.h \
    $$PWD/codecompletefailedexception.h \
    $$PWD/clangcodecompleteresults.h \
    $$PWD/codecompletionsextractor.h \
    $$PWD/unsavedfiles.h \
    $$PWD/translationunitfilenotexitsexception.h

SOURCES += clangipcserver.cpp \
           codecompleter.cpp \
    $$PWD/clangstring.cpp \
    $$PWD/translationunit.cpp \
    $$PWD/translationunitisnullexception.cpp \
    $$PWD/codecompletefailedexception.cpp \
    $$PWD/clangcodecompleteresults.cpp \
    $$PWD/codecompletionsextractor.cpp \
    $$PWD/unsavedfiles.cpp \
    $$PWD/translationunitfilenotexitsexception.cpp
