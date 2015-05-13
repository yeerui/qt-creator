import qbs 1.0
import qbs.File
import qbs.Process
import QtcProcessOutputReader

QtcTool {
    name: "codemodelbackend"

    Depends { name: "CodeModelBackEndIpc" }

    Group {
        prefix: "ipcsource/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    files: [ "codemodelbackendmain.cpp" ]

    property string llvmInstallDirFromEnv: qbs.getEnv("LLVM_INSTALL_DIR")

    property bool clangCompletion: true
    property bool clangHighlighting: true
    property bool clangIndexing: false

    property string llvmConfig: {
        var llvmConfigVariants = [
            "llvm-config", "llvm-config-3.2", "llvm-config-3.3", "llvm-config-3.4",
            "llvm-config-3.5", "llvm-config-3.6", "llvm-config-4.0", "llvm-config-4.1"
        ];

        // Prefer llvm-config* from LLVM_INSTALL_DIR
        if (llvmInstallDirFromEnv) {
            for (var i = 0; i < llvmConfigVariants.length; ++i) {
                var variant = llvmInstallDirFromEnv + "/bin/" + llvmConfigVariants[i];
                if (File.exists(variant))
                    return variant;
            }
        }

        // Find llvm-config* in PATH
        var pathListString = qbs.getEnv("PATH");
        var separator = qbs.hostOS.contains("windows") ? ";" : ":";
        var pathList = pathListString.split(separator);
        for (var i = 0; i < llvmConfigVariants.length; ++i) {
            for (var j = 0; j < pathList.length; ++j) {
                var variant = pathList[j] + "/" + llvmConfigVariants[i];
                if (File.exists(variant))
                    return variant;
            }
        }

        return undefined;
    }
    condition: llvmConfig

    property string llvmIncludeDir: QtcProcessOutputReader.readOutput(llvmConfig, ["--includedir"])
    property string llvmLibDir: QtcProcessOutputReader.readOutput(llvmConfig, ["--libdir"])
    property string llvmVersion: QtcProcessOutputReader.readOutput(llvmConfig, ["--version"])
        .replace(/(\d+\.\d+\.\d+).*/, "$1")

    cpp.includePaths: base.concat(["ipcsource", llvmIncludeDir])
    cpp.libraryPaths: base.concat(llvmLibDir)
    cpp.rpaths: cpp.libraryPaths

    property string llvmLib: "clang"
    property stringList additionalLibraries: qbs.targetOS.contains("windows")
                                             ? ["advapi32", "shell32"] : []
    cpp.dynamicLibraries: base.concat(llvmLib).concat(additionalLibraries)

    cpp.defines: {
        var defines = base;
        defines.push('CLANG_VERSION="' + llvmVersion + '"');
        defines.push('CLANG_RESOURCE_DIR="' + llvmLibDir + '/clang/' + llvmVersion + '/include"');
        if (clangCompletion)
            defines.push("CLANG_COMPLETION");
        if (clangHighlighting)
            defines.push("CLANG_HIGHLIGHTING");
        if (clangIndexing)
            defines.push("CLANG_INDEXING");
        return defines;
    }


}


