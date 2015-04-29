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
    $$PWD/translationunitfilenotexitsexception.h \
    $$PWD/project.h \
    $$PWD/projects.h \
    $$PWD/projectdonotexistsexception.h \
    $$PWD/translationunits.h \
    $$PWD/translationunitparseerrorexception.h \
    $$PWD/translationunitdoesnotexistsexception.h

SOURCES += clangipcserver.cpp \
           codecompleter.cpp \
    $$PWD/clangstring.cpp \
    $$PWD/translationunit.cpp \
    $$PWD/translationunitisnullexception.cpp \
    $$PWD/codecompletefailedexception.cpp \
    $$PWD/clangcodecompleteresults.cpp \
    $$PWD/codecompletionsextractor.cpp \
    $$PWD/unsavedfiles.cpp \
    $$PWD/translationunitfilenotexitsexception.cpp \
    $$PWD/project.cpp \
    $$PWD/projects.cpp \
    $$PWD/projectdonotexistsexception.cpp \
    $$PWD/translationunits.cpp \
    $$PWD/translationunitparseerrorexception.cpp \
    $$PWD/translationunitdoesnotexistsexception.cpp
