import qbs 1.0

QtcLibrary {
    name: "Sqlite"

    cpp.includePaths: base.concat("../3rdparty/sqlite")
    cpp.defines: base.concat([
        "CPLUSPLUS_BUILD_LIB"
    ])
    cpp.optimization: "fast"


    Group {
        name: "ThirdSqlite"
        prefix: "../3rdparty/sqlite/"
        files: [
            "sqlite3.c",
            "sqlite3.h",
            "sqlite3ext.h",
        ]
    }

    Export {
        cpp.includePaths: [
            "../3rdparty/sqlite"
        ]
    }
}
