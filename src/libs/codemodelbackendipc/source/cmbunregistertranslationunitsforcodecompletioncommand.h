#ifndef CODEMODELBACKEND_UNRegisterTranslationUnitForCodeCompletionCommand_H
#define CODEMODELBACKEND_UNRegisterTranslationUnitForCodeCompletionCommand_H

#include "codemodelbackendipc_global.h"

#include "filecontainer.h"

#include <QVector>
#include <QMetaType>

namespace CodeModelBackEnd {

class CMBIPC_EXPORT UnregisterTranslationUnitsForCodeCompletionCommand
{
    friend CMBIPC_EXPORT QDataStream &operator<<(QDataStream &out, const UnregisterTranslationUnitsForCodeCompletionCommand &command);
    friend CMBIPC_EXPORT QDataStream &operator>>(QDataStream &in, UnregisterTranslationUnitsForCodeCompletionCommand &command);
    friend CMBIPC_EXPORT bool operator == (const UnregisterTranslationUnitsForCodeCompletionCommand &first, const UnregisterTranslationUnitsForCodeCompletionCommand &second);
    friend CMBIPC_EXPORT bool operator < (const UnregisterTranslationUnitsForCodeCompletionCommand &first, const UnregisterTranslationUnitsForCodeCompletionCommand &second);
    friend void PrintTo(const UnregisterTranslationUnitsForCodeCompletionCommand &command, ::std::ostream* os);

public:
    UnregisterTranslationUnitsForCodeCompletionCommand() = default;
    UnregisterTranslationUnitsForCodeCompletionCommand(const QVector<FileContainer> &fileContainers);

    const QVector<FileContainer> &fileContainers() const;

private:
    QVector<FileContainer> fileContainers_;
};

CMBIPC_EXPORT QDataStream &operator<<(QDataStream &out, const UnregisterTranslationUnitsForCodeCompletionCommand &command);
CMBIPC_EXPORT QDataStream &operator>>(QDataStream &in, UnregisterTranslationUnitsForCodeCompletionCommand &command);
CMBIPC_EXPORT bool operator == (const UnregisterTranslationUnitsForCodeCompletionCommand &first, const UnregisterTranslationUnitsForCodeCompletionCommand &second);
CMBIPC_EXPORT bool operator < (const UnregisterTranslationUnitsForCodeCompletionCommand &first, const UnregisterTranslationUnitsForCodeCompletionCommand &second);

CMBIPC_EXPORT QDebug operator <<(QDebug debug, const UnregisterTranslationUnitsForCodeCompletionCommand &command);
#ifdef CODEMODELBACKEND_TESTS
void PrintTo(const UnregisterTranslationUnitsForCodeCompletionCommand &command, ::std::ostream* os);
#endif
} // namespace CodeModelBackEnd

Q_DECLARE_METATYPE(CodeModelBackEnd::UnregisterTranslationUnitsForCodeCompletionCommand)

#endif // CODEMODELBACKEND_UNRegisterTranslationUnitForCodeCompletionCommand_H
