#ifndef CODEMODELBACKEND_UNRegisterFilesForCodeCompletionCommand_H
#define CODEMODELBACKEND_UNRegisterFilesForCodeCompletionCommand_H

#include <qmetatype.h>

#include <QVector>
#include <QByteArray>

#include "codemodelbackendipc_global.h"

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
    UnregisterFilesForCodeCompletionCommand(const QVector<QByteArray> &filePaths);

    const QVector<QByteArray> &filePaths() const;

private:
    QVector<QByteArray> filePaths_;
};

QDataStream &operator<<(QDataStream &out, const UnregisterFilesForCodeCompletionCommand &command);
QDataStream &operator>>(QDataStream &in, UnregisterFilesForCodeCompletionCommand &command);
bool operator == (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second);
bool operator < (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second);

QDebug operator <<(QDebug debug, const UnregisterFilesForCodeCompletionCommand &command);
void PrintTo(const UnregisterFilesForCodeCompletionCommand &command, ::std::ostream* os);
} // namespace CodeModelBackEnd

Q_DECLARE_METATYPE(CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand)

#endif // CODEMODELBACKEND_UNRegisterFilesForCodeCompletionCommand_H
