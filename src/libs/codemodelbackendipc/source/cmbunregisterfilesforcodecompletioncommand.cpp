#include "cmbunregisterfilesforcodecompletioncommand.h"

#include <QtDebug>

#ifdef CODEMODELBACKEND_TESTS
#include <gtest/gtest-printers.h>
#endif

namespace CodeModelBackEnd {


UnregisterFilesForCodeCompletionCommand::UnregisterFilesForCodeCompletionCommand(const QVector<FileContainer> &fileContainers)
    : fileContainers_(fileContainers)
{
}

const QVector<FileContainer> &UnregisterFilesForCodeCompletionCommand::fileContainers() const
{
    return fileContainers_;
}

QDataStream &operator<<(QDataStream &out, const UnregisterFilesForCodeCompletionCommand &command)
{
    out << command.fileContainers_;

    return out;
}

QDataStream &operator>>(QDataStream &in, UnregisterFilesForCodeCompletionCommand &command)
{
    in >> command.fileContainers_;

    return in;
}

bool operator == (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second)
{
    return first.fileContainers_ == second.fileContainers_;
}

bool operator < (const UnregisterFilesForCodeCompletionCommand &first, const UnregisterFilesForCodeCompletionCommand &second)
{
    return first.fileContainers_ < second.fileContainers_;
}

QDebug operator <<(QDebug debug, const UnregisterFilesForCodeCompletionCommand &command)
{
    debug.nospace() << "UnregisterFileForCodeCompletion(";

    for (const FileContainer &fileContainer : command.fileContainers())
        debug.nospace() << fileContainer << ", ";

    debug.nospace() << ")";

    return debug;
}

#ifdef CODEMODELBACKEND_TESTS
void PrintTo(const UnregisterFilesForCodeCompletionCommand &command, ::std::ostream* os)
{
    *os << "UnregisterFileForCodeCompletion(";

    for (const FileContainer &fileContainer : command.fileContainers())
        *os << ::testing::PrintToString(fileContainer) << ", ";

    *os << ")";
}
#endif


} // namespace CodeModelBackEnd

