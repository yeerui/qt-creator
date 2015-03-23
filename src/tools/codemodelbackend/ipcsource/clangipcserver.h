#ifndef CLANGIPCSERVER_H
#define CLANGIPCSERVER_H

#include "ipcserverinterface.h"

#include <QMap>
#include <QByteArray>


namespace CodeModelBackEnd {

class ClangIpcServer : public IpcServerInterface
{
public:
    void end() override;
    void registerFilesForCodeCompletion(const RegisterFilesForCodeCompletionCommand &command) override;
    void unregisterFilesForCodeCompletion(const UnregisterFilesForCodeCompletionCommand &command) override;
    void completeCode(const CompleteCodeCommand &command) override;

};

} // namespace CodeModelBackEnd
#endif // CLANGIPCSERVER_H
