#include "clangipcserver.h"

#include <QCoreApplication>

#include <cmbregisterfilesforcodecompletioncommand.h>
#include <cmbunregisterfilesforcodecompletioncommand.h>
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

void ClangIpcServer::registerFilesForCodeCompletion(const CodeModelBackEnd::RegisterFilesForCodeCompletionCommand &command)
{
    try {
        translationUnits.createOrUpdate(command.fileContainers());
        unsavedFiles.createOrUpdate(command.fileContainers());
    } catch (const TranslationUnitFileNotExitsException &exception) {
        qWarning() << "Error in ClangIpcServer::registerFilesForCodeCompletion: Tried to access a null TranslationUnit!";
    } catch (const TranslationUnitIsNullException &exception) {
        qWarning() << "Error in ClangIpcServer::registerFilesForCodeCompletion: File for TranslationUnit doesn't exits!";
    } catch (const ProjectDoesNotExistsException &exception) {
        client()->projectDoesNotExists(ProjectDoesNotExistsCommand(exception.projectFilePath()));
    }
}

void ClangIpcServer::unregisterFilesForCodeCompletion(const CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand &command)
{
    try {
        translationUnits.remove(command.fileContainers());
    } catch (const TranslationUnitDoesNotExistsException &exception) {
        client()->translationUnitDoesNotExists(TranslationUnitDoesNotExistsCommand(exception.fileContainer()));
    } catch(const TranslationUnitIsNullException &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterFilesForCodeCompletion: Tried to access a null TranslationUnit!";
    } catch (const ProjectDoesNotExistsException &exception) {
        client()->projectDoesNotExists(ProjectDoesNotExistsCommand(exception.projectFilePath()));
    }
}

void ClangIpcServer::registerProjectsForCodeCompletion(const RegisterProjectsForCodeCompletionCommand &command)
{
    try {
        projects.createOrUpdate(command.projectContainers());
    } catch (const TranslationUnitIsNullException &exception) {
        qWarning() << "Error in ClangIpcServer::registerProjectsForCodeCompletion: Tried to access a null TranslationUnit!";
    }
}

void ClangIpcServer::unregisterProjectsForCodeCompletion(const UnregisterProjectsForCodeCompletionCommand &command)
{
    try {
        projects.remove(command.filePaths());
    } catch (const TranslationUnitIsNullException &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterProjectsForCodeCompletion: Tried to access a null TranslationUnit!";
    } catch (const ProjectDoesNotExistsException &exception) {
        client()->projectDoesNotExists(ProjectDoesNotExistsCommand(exception.projectFilePath()));
    }
}

void ClangIpcServer::completeCode(const CodeModelBackEnd::CompleteCodeCommand &command)
{
    try {
        CodeCompleter codeCompleter(translationUnits.translationUnit(command.filePath(), command.projectFilePath()));

        const auto codeCompletions = codeCompleter.complete(command.line(), command.column());

        client()->codeCompleted(CodeCompletedCommand(codeCompletions));
    } catch (const TranslationUnitDoesNotExistsException &exception) {
        client()->translationUnitDoesNotExists(TranslationUnitDoesNotExistsCommand(exception.fileContainer()));
    } catch (const TranslationUnitIsNullException &exception) {
        qWarning() << "Error in ClangIpcServer::completeCode: Tried to access a null TranslationUnit!";
    } catch (const ProjectDoesNotExistsException &exception) {
        client()->projectDoesNotExists(ProjectDoesNotExistsCommand(exception.projectFilePath()));
    } catch (const TranslationUnitParseErrorException &exception) {
        qWarning() << "Error in ClangIpcServer::completeCode: Parse error for file"
                   << exception.filePath()
                   << " in project"
                   << exception.projectFilePath()
                   << "!";
    }
}

}
