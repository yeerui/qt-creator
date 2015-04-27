#include "clangipcserver.h"

#include <QCoreApplication>

#include <cmbregisterfilesforcodecompletioncommand.h>
#include <cmbunregisterfilesforcodecompletioncommand.h>
#include <cmbregisterprojectsforcodecompletioncommand.h>
#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>

#include "translationunits.h"
#include "codecompleter.h"
#include "translationunitdonotexistsexception.h"

#include <QDebug>

namespace CodeModelBackEnd {

ClangIpcServer::ClangIpcServer()
    : translationUnits(projects, unsavedFiles)
{
}

void ClangIpcServer::end()
{
    QCoreApplication::exit();
}

void ClangIpcServer::registerFilesForCodeCompletion(const CodeModelBackEnd::RegisterFilesForCodeCompletionCommand &command)
{
    translationUnits.createOrUpdate(command.fileContainers());
    unsavedFiles.createOrUpdate(command.fileContainers());
}

void ClangIpcServer::unregisterFilesForCodeCompletion(const CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand &/*command*/)
{

}

void ClangIpcServer::registerProjectsForCodeCompletion(const RegisterProjectsForCodeCompletionCommand &command)
{
    projects.createOrUpdate(command.projectContainers());
}

void ClangIpcServer::unregisterProjectsForCodeCompletion(const UnregisterProjectsForCodeCompletionCommand &command)
{

}

void ClangIpcServer::completeCode(const CodeModelBackEnd::CompleteCodeCommand &command)
{
    CodeCompleter codeCompleter(translationUnits.translationUnit(command.filePath(), command.projectFilePath()));

    client()->codeCompleted(CodeCompletedCommand(codeCompleter.complete(command.line(), command.column())));
}

}
