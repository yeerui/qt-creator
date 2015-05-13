#include "clangipcserver.h"

#include <QCoreApplication>

#include <cmbregistertranslationunitsforcodecompletioncommand.h>
#include <cmbunregistertranslationunitsforcodecompletioncommand.h>
#include <cmbregisterprojectsforcodecompletioncommand.h>
#include <cmbunregisterprojectsforcodecompletioncommand.h>
#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>
#include <translationunitdoesnotexistcommand.h>
#include <projectpartsdonotexistcommand.h>

#include "translationunits.h"
#include "codecompleter.h"
#include "translationunitdoesnotexistexception.h"
#include "translationunitisnullexception.h"
#include "translationunitfilenotexitexception.h"
#include "projectpartsdonotexistexception.h"
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
    } catch (const ProjectPartDoNotExistException &exception) {
        client()->projectPartsDoNotExist(ProjectPartsDoNotExistCommand(exception.projectPartIds()));
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
    } catch (const ProjectPartDoNotExistException &exception) {
        client()->projectPartsDoNotExist(ProjectPartsDoNotExistCommand(exception.projectPartIds()));
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterTranslationUnitsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::registerProjectPartsForCodeCompletion(const RegisterProjectPartsForCodeCompletionCommand &command)
{
    try {
        projects.createOrUpdate(command.projectContainers());
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::registerProjectPartsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::unregisterProjectPartsForCodeCompletion(const UnregisterProjectPartsForCodeCompletionCommand &command)
{
    try {
        projects.remove(command.filePaths());
    } catch (const ProjectPartDoNotExistException &exception) {
        client()->projectPartsDoNotExist(ProjectPartsDoNotExistCommand(exception.projectPartIds()));
    } catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterProjectPartsForCodeCompletion:" << exception.what();
    }
}

void ClangIpcServer::completeCode(const CodeModelBackEnd::CompleteCodeCommand &command)
{
    try {
        CodeCompleter codeCompleter(translationUnits.translationUnit(command.filePath(), command.projectPartId()));

        const auto codeCompletions = codeCompleter.complete(command.line(), command.column());

        client()->codeCompleted(CodeCompletedCommand(codeCompletions, command.ticketNumber()));
    } catch (const TranslationUnitDoesNotExistException &exception) {
        client()->translationUnitDoesNotExist(TranslationUnitDoesNotExistCommand(exception.fileContainer()));
    } catch (const ProjectPartDoNotExistException &exception) {
        client()->projectPartsDoNotExist(ProjectPartsDoNotExistCommand(exception.projectPartIds()));
    }  catch (const std::exception &exception) {
        qWarning() << "Error in ClangIpcServer::unregisterProjectPartsForCodeCompletion:" << exception.what();
    }
}

}
