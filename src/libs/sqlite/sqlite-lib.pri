contains(CONFIG, dll) {
    DEFINES += BUILD_SQLITE_LIBRARY
} else {
    DEFINES += BUILD_SQLITE_STATIC_LIBRARY
}

unix:LIBS += -ldl

include(../3rdparty/sqlite/sqlite.pri)
include(source/sqlite-source.pri)

DEFINES += SQLITE_THREADSAFE=2 SQLITE_ENABLE_FTS4 SQLITE_ENABLE_FTS3_PARENTHESIS SQLITE_ENABLE_UNLOCK_NOTIFY SQLITE_ENABLE_COLUMN_METADATA

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols
