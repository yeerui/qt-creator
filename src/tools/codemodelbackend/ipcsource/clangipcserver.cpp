#include "clangipcserver.h"

#include <QCoreApplication>

#include <cmbregistertranslationunitsforcodecompletioncommand.h>
#include <cmbunregistertranslationunitsforcodecompletioncommand.h>
#include <cmbregisterprojectsforcodecompletioncommand.h>
#include <cmbunregisterprojectsforcodecompletioncommand.h>
#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>
#include <translationunitdoesnotexistscommand.h>
#include <projectdoesnotexistscommand.h>

#include "translationunits.h"
#include "codecompleter.h"
#include "translationunitdoesnotexistsexception.h"
#include "translationunitisnullexception.h"
#include "translationunitfilenotexitsexception.h"
#include "projectdoesnotexistsexception.h"
#include "translationunitparseerrorexception.h"

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

void ClangIpcServer::registerTranslationUnitsForCodeCompletion(const CodeModelBackEnd::RegisterTranslationUnitForCodeCompletionCommand &command)
{
    try {
        translationUnits.createOrUpdate(command.fileContainers());
        unsavedFiles.createOrUpdate(command.fileContainers());
    } catch (const ProjectDoesNotExistException &exception) {
        client()->projectDoesNotExist(ProjectDoesNotExistCommand(exception.projectFilePath()));
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::registerTranslationUnitsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::unregisterTranslationUnitsForCodeCompletion(const CodeModelBackEnd::UnregisterTranslationUnitsForCodeCompletionCommand &command)
{
    try {
        translationUnits.remove(command.fileContainers());
    } catch (const TranslationUnitDoesNotExistException &exception) {
        client()->translationUnitDoesNotExist(TranslationUnitDoesNotExistCommand(exception.fileContainer()));
    } catch (const ProjectDoesNotExistException &exception) {
        client()->projectDoesNotExist(ProjectDoesNotExistCommand(exception.projectFilePath()));
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterTranslationUnitsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::registerProjectsForCodeCompletion(const RegisterProjectsForCodeCompletionCommand &command)
{
    try {
        projects.createOrUpdate(command.projectContainers());
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::registerProjectsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::unregisterProjectsForCodeCompletion(const UnregisterProjectsForCodeCompletionCommand &command)
{
    try {
        projects.remove(command.filePaths());
    } catch (const ProjectDoesNotExistException &exception) {
        client()->projectDoesNotExist(ProjectDoesNotExistCommand(exception.projectFilePath()));
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterProjectsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::completeCode(const CodeModelBackEnd::CompleteCodeCommand &command)
{
    try {
        CodeCompleter codeCompleter(translationUnits.translationUnit(command.filePath(), command.projectFilePath()));

        const auto codeCompletions = codeCompleter.complete(command.line(), command.column());

        client()->codeCompleted(CodeCompletedCommand(codeCompletions, command.ticketNumber()));
    } catch (const TranslationUnitDoesNotExistException &exception) {
        client()->translationUnitDoesNotExist(TranslationUnitDoesNotExistCommand(exception.fileContainer()));
    } catch (const ProjectDoesNotExistException &exception) {
        client()->projectDoesNotExist(ProjectDoesNotExistCommand(exception.projectFilePath()));
    }  catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterProjectsForCodeCompletion:" << exception.what();
    }
}

}
