#include "cmbunregisterfilesforcodecompletioncommand.h"

#include <QtDebug>

namespace CodeModelBackEnd {


UnregisterFilesForCodeCompletionCommand::UnregisterFilesForCodeCompletionCommand(const Utf8StringVector &filePaths)
    : filePaths_(filePaths)
{
}

const Utf8StringVector &UnregisterFilesForCodeCompletionCommand::filePaths() const
{
    return filePaths_;
}

QDataStream &operator<<(QDataStream &out, const UnregisterFilesForCodeCompletionCommand &command)
{
    out << command.filePaths_;

    return out;
}

QDataStream &operator>>(QDataStream &in, UnregisterFilesForCodeCompletionCommand &command)
{
    in >> command.filePaths_;

    return in;
}

bool operator == (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second)
{
    return first.filePaths_ == second.filePaths_;
}

bool operator < (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second)
{
    return first.filePaths_ < second.filePaths_;
}

QDebug operator <<(QDebug debug, const UnregisterFilesForCodeCompletionCommand &command)
{
    debug.nospace() << "UnregisterFileForCodeCompletion(";

    for (const Utf8String &fileNames_ : command.filePaths())
        debug.nospace() << fileNames_ << ", ";

    debug.nospace() << ")";

    return debug;
}

void PrintTo(const UnregisterFilesForCodeCompletionCommand &command, ::std::ostream* os)
{
    *os << "UnregisterFileForCodeCompletion(";

    for (const Utf8String &fileNames_ : command.filePaths())
        *os << fileNames_.constData() << ", ";

    *os << ")";
}

} // namespace CodeModelBackEnd

