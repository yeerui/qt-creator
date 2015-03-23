contains(CONFIG, dll) {
    DEFINES += CODEMODELBACKENDIPC_BUILD_LIB
} else {
    DEFINES += CODEMODELBACKENDIPC_BUILD_STATIC_LIB
}

QT += network

DEFINES += CODEMODELBACKENDIPC_LIBRARY

include(source/codemodelbackendipc-source.pri)

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols
