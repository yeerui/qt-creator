#include "cmbunregistertranslationunitsforcodecompletioncommand.h"

#include <QtDebug>

#ifdef CODEMODELBACKEND_TESTS
#include <gtest/gtest-printers.h>
#endif

namespace CodeModelBackEnd {


UnregisterTranslationUnitsForCodeCompletionCommand::UnregisterTranslationUnitsForCodeCompletionCommand(const QVector<FileContainer> &fileContainers)
    : fileContainers_(fileContainers)
{
}

const QVector<FileContainer> &UnregisterTranslationUnitsForCodeCompletionCommand::fileContainers() const
{
    return fileContainers_;
}

QDataStream &operator<<(QDataStream &out, const UnregisterTranslationUnitsForCodeCompletionCommand &command)
{
    out << command.fileContainers_;

    return out;
}

QDataStream &operator>>(QDataStream &in, UnregisterTranslationUnitsForCodeCompletionCommand &command)
{
    in >> command.fileContainers_;

    return in;
}

bool operator == (const UnregisterTranslationUnitsForCodeCompletionCommand &first, const UnregisterTranslationUnitsForCodeCompletionCommand &second)
{
    return first.fileContainers_ == second.fileContainers_;
}

bool operator < (const UnregisterTranslationUnitsForCodeCompletionCommand &first, const UnregisterTranslationUnitsForCodeCompletionCommand &second)
{
    return first.fileContainers_ < second.fileContainers_;
}

QDebug operator <<(QDebug debug, const UnregisterTranslationUnitsForCodeCompletionCommand &command)
{
    debug.nospace() << "UnregisterFileForCodeCompletion(";

    for (const FileContainer &fileContainer : command.fileContainers())
        debug.nospace() << fileContainer << ", ";

    debug.nospace() << ")";

    return debug;
}

#ifdef CODEMODELBACKEND_TESTS
void PrintTo(const UnregisterTranslationUnitsForCodeCompletionCommand &command, ::std::ostream* os)
{
    *os << "UnregisterFileForCodeCompletion(";

    for (const FileContainer &fileContainer : command.fileContainers())
        *os << ::testing::PrintToString(fileContainer) << ", ";

    *os << ")";
}
#endif


} // namespace CodeModelBackEnd

