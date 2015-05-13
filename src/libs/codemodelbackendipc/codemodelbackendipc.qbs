import qbs 1.0

QtcLibrary {
    name: "CodeModelBackEndIpc"

    Depends { name: "Qt.network" }

    cpp.defines: base.concat([
                                 'CODEMODELBACKENDIPC_LIBRARY',
                                 'SERVER_NAME="org.qt-project.QtCreator.ClangCodeModel"'
                             ])

    files: [
        source/cmbalivecommand.cpp,
        source/cmbalivecommand.h,
        source/cmbcodecompletedcommand.cpp,
        source/cmbcodecompletedcommand.h,
        source/cmbcommands.cpp,
        source/cmbcommands.h,
        source/cmbcompletecodecommand.cpp,
        source/cmbcompletecodecommand.h,
        source/cmbechocommand.cpp,
        source/cmbechocommand.h,
        source/cmbendcommand.cpp,
        source/cmbendcommand.h,
        source/cmbregisterprojectsforcodecompletioncommand.cpp,
        source/cmbregisterprojectsforcodecompletioncommand.h,
        source/cmbregistertranslationunitsforcodecompletioncommand.cpp,
        source/cmbregistertranslationunitsforcodecompletioncommand.cpp.orig,
        source/cmbregistertranslationunitsforcodecompletioncommand.h,
        source/cmbunregisterprojectsforcodecompletioncommand.cpp,
        source/cmbunregisterprojectsforcodecompletioncommand.h,
        source/cmbunregistertranslationunitsforcodecompletioncommand.cpp,
        source/cmbunregistertranslationunitsforcodecompletioncommand.h,
        source/codecompletionchunk.cpp, source/codecompletionchunk.h,
        source/codecompletion.cpp,
        source/codecompletion.h,
        source/codemodelbackendipc_global.cpp,
        source/codemodelbackendipc_global.h,
        source/codemodelbackendipc.pro.user,
        source/connectionclient.cpp,
        source/connectionclient.h,
        source/connectionserver.cpp,
        source/connectionserver.h,
        source/filecontainer.cpp,
        source/filecontainer.h,
        source/ipcclientdispatcher.cpp,
        source/ipcclientdispatcher.h,
        source/ipcclientinterface.cpp,
        source/ipcclientinterface.h,
        source/ipcclientproxy.cpp,
        source/ipcclientproxy.h,
        source/ipcinterface.cpp,
        source/ipcinterface.h,
        source/ipcserver.cpp,
        source/ipcserver.h,
        source/ipcserverinterface.cpp,
        source/ipcserverinterface.h,
        source/ipcserverproxy.cpp,
        source/ipcserverproxy.h,
        source/projectpartcontainer.cpp,
        source/projectpartcontainer.h,
        source/projectpartsdoesnotexistcommand.cpp,
        source/projectpartsdoesnotexistcommand.h,
        source/readcommandblock.cpp,
        source/readcommandblock.h,
        source/translationunitdoesnotexistcommand.cpp,
        source/translationunitdoesnotexistcommand.h,
        source/writecommandblock.cpp,
        source/writecommandblock.h
    ]

    Export {
        Depends { name: "Qt.network" }
    }
}


