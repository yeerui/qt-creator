#ifndef CLANGIPCSERVER_H
#define CLANGIPCSERVER_H

#include "ipcserverinterface.h"

#include "translationunit.h"
#include "project.h"
#include "unsavedfiles.h"
#include "projects.h"
#include "translationunits.h"

#include <utf8string.h>

#include <QMap>

namespace CodeModelBackEnd {

class ClangIpcServer : public IpcServerInterface
{
public:
    ClangIpcServer();

    void end() override;
    void registerFilesForCodeCompletion(const RegisterFilesForCodeCompletionCommand &command) override;
    void unregisterFilesForCodeCompletion(const UnregisterFilesForCodeCompletionCommand &command) override;
    void registerProjectsForCodeCompletion(const RegisterProjectsForCodeCompletionCommand &command) override;
    void unregisterProjectsForCodeCompletion(const UnregisterProjectsForCodeCompletionCommand &command) override;
    void completeCode(const CompleteCodeCommand &command) override;

private:
    Projects projects;
    UnsavedFiles unsavedFiles;
    TranslationUnits translationUnits;
};

} // namespace CodeModelBackEnd
#endif // CLANGIPCSERVER_H
