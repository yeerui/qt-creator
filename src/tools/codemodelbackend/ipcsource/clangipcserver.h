#ifndef CLANGIPCSERVER_H
#define CLANGIPCSERVER_H

#include "ipcserverinterface.h"

#include "translationunit.h"
#include "projectpart.h"
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
    void registerTranslationUnitsForCodeCompletion(const RegisterTranslationUnitForCodeCompletionCommand &command) override;
    void unregisterTranslationUnitsForCodeCompletion(const UnregisterTranslationUnitsForCodeCompletionCommand &command) override;
    void registerProjectPartsForCodeCompletion(const RegisterProjectPartsForCodeCompletionCommand &command) override;
    void unregisterProjectPartsForCodeCompletion(const UnregisterProjectPartsForCodeCompletionCommand &command) override;
    void completeCode(const CompleteCodeCommand &command) override;

private:
    ProjectParts projects;
    UnsavedFiles unsavedFiles;
    TranslationUnits translationUnits;
};

} // namespace CodeModelBackEnd
#endif // CLANGIPCSERVER_H
