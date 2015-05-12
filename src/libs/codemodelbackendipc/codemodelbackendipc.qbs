import qbs 1.0

QtcLibrary {
    name: "CodeModelBackEndIpc"

    Depends { name: "Qt.network" }
    Depends { name: "Sqlite" }

    cpp.defines: base.concat("CODEMODELBACKENDIPC_LIBRARY")
    cpp.includePaths: base.concat("source")

    Group {
        prefix: "source/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    Export {
        Depends { name: "Sqlite" }
        Depends { name: "Qt.network" }
        cpp.includePaths: [
            "source"
        ]
    }
}


