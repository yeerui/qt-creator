#include "clangipcserver.h"

#include <QCoreApplication>

#include <cmbregisterfilesforcodecompletioncommand.h>
#include <cmbunregisterfilesforcodecompletioncommand.h>
#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>

#include <QtDebug>


namespace CodeModelBackEnd {

void ClangIpcServer::end()
{
    QCoreApplication::exit();
}

void ClangIpcServer::registerFilesForCodeCompletion(const CodeModelBackEnd::RegisterFilesForCodeCompletionCommand &/*command*/)
{

}

void ClangIpcServer::unregisterFilesForCodeCompletion(const CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand &/*command*/)
{

}

void ClangIpcServer::completeCode(const CodeModelBackEnd::CompleteCodeCommand &/*command*/)
{
    client()->codeCompleted(CodeCompletedCommand());
}

}
