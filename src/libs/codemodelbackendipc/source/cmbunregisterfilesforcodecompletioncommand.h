#ifndef CODEMODELBACKEND_UNRegisterFilesForCodeCompletionCommand_H
#define CODEMODELBACKEND_UNRegisterFilesForCodeCompletionCommand_H

#include "codemodelbackendipc_global.h"

#include <utf8stringvector.h>

#include <QMetaType>

namespace CodeModelBackEnd {

class CMBIPC_EXPORT UnregisterFilesForCodeCompletionCommand
{
    friend QDataStream &operator<<(QDataStream &out, const UnregisterFilesForCodeCompletionCommand &command);
    friend QDataStream &operator>>(QDataStream &in, UnregisterFilesForCodeCompletionCommand &command);
    friend bool operator == (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second);
    friend bool operator < (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second);
    friend void PrintTo(const UnregisterFilesForCodeCompletionCommand &command, ::std::ostream* os);

public:
    UnregisterFilesForCodeCompletionCommand() = default;
    UnregisterFilesForCodeCompletionCommand(const Utf8StringVector &filePaths);

    const Utf8StringVector &filePaths() const;

private:
    Utf8StringVector filePaths_;
};

CMBIPC_EXPORT QDataStream &operator<<(QDataStream &out, const UnregisterFilesForCodeCompletionCommand &command);
CMBIPC_EXPORT QDataStream &operator>>(QDataStream &in, UnregisterFilesForCodeCompletionCommand &command);
CMBIPC_EXPORT bool operator == (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second);
CMBIPC_EXPORT bool operator < (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second);

CMBIPC_EXPORT QDebug operator <<(QDebug debug, const UnregisterFilesForCodeCompletionCommand &command);
void PrintTo(const UnregisterFilesForCodeCompletionCommand &command, ::std::ostream* os);
} // namespace CodeModelBackEnd

Q_DECLARE_METATYPE(CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand)

#endif // CODEMODELBACKEND_UNRegisterFilesForCodeCompletionCommand_H
