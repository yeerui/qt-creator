/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"

#include "gtest-qt-printing.h"

#include <ipcclientproxy.h>
#include <ipcserverproxy.h>
#include <clangipcserver.h>
#include <translationunitdoesnotexistsexception.h>
#include <translationunitparseerrorexception.h>

#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>
#include <cmbechocommand.h>
#include <cmbregistertranslationunitsforcodecompletioncommand.h>
#include <cmbunregistertranslationunitsforcodecompletioncommand.h>
#include <cmbregisterprojectsforcodecompletioncommand.h>
#include <cmbunregisterprojectsforcodecompletioncommand.h>
#include <translationunitdoesnotexistscommand.h>
#include <projectdoesnotexistscommand.h>

#include <QBuffer>
#include <QFile>
#include <projectdoesnotexistscommand.h>

#include "mockipclient.h"

using testing::Property;
using testing::Contains;
using testing::Not;
using testing::Eq;

namespace {

using CodeModelBackEnd::RegisterTranslationUnitForCodeCompletionCommand;
using CodeModelBackEnd::UnregisterTranslationUnitsForCodeCompletionCommand;
using CodeModelBackEnd::RegisterProjectsForCodeCompletionCommand;
using CodeModelBackEnd::UnregisterProjectsForCodeCompletionCommand;
using CodeModelBackEnd::CompleteCodeCommand;
using CodeModelBackEnd::CodeCompletedCommand;
using CodeModelBackEnd::CodeCompletion;
using CodeModelBackEnd::FileContainer;
using CodeModelBackEnd::ProjectContainer;
using CodeModelBackEnd::TranslationUnitDoesNotExistCommand;
using CodeModelBackEnd::ProjectDoesNotExistCommand;

class ClangIpcServer : public ::testing::Test
{
protected:
    void SetUp() override;

    void registerFiles();
    void registerProject();
    void changeProjectArguments();
    void changeProjectArgumentsToWrongValues();
    static const Utf8String unsavedContent(const QString &unsavedFilePath);

protected:
    MockIpcClient mockIpcClient;
    CodeModelBackEnd::ClangIpcServer clangServer;
    const Utf8String projectFilePath = Utf8StringLiteral("pathToProject.pro");
    const Utf8String functionTestFilePath = Utf8StringLiteral("data/complete_extractor_function.cpp");
    const Utf8String variableTestFilePath = Utf8StringLiteral("data/complete_extractor_variable.cpp");
    const QString unsavedTestFilePath = QStringLiteral("data/complete_extractor_function_unsaved.cpp");
    const QString updatedUnsavedTestFilePath = QStringLiteral("data/complete_extractor_function_unsaved_2.cpp");
    const Utf8String parseErrorTestFilePath = Utf8StringLiteral("data/complete_translationunit_parse_error.cpp");
};


void ClangIpcServer::SetUp()
{
    clangServer.addClient(&mockIpcClient);
    registerProject();
    registerFiles();
}

void ClangIpcServer::registerFiles()
{
    RegisterTranslationUnitForCodeCompletionCommand command({FileContainer(functionTestFilePath, projectFilePath, unsavedContent(unsavedTestFilePath), true),
                                                   FileContainer(variableTestFilePath, projectFilePath)});

    clangServer.registerTranslationUnitsForCodeCompletion(command);
}

void ClangIpcServer::registerProject()
{
    RegisterProjectsForCodeCompletionCommand command({ProjectContainer(projectFilePath)});

    clangServer.registerProjectsForCodeCompletion(command);
}

void ClangIpcServer::changeProjectArguments()
{
    RegisterProjectsForCodeCompletionCommand command({ProjectContainer(projectFilePath, {Utf8StringLiteral("-DArgumentDefinition")})});

    clangServer.registerProjectsForCodeCompletion(command);
}

void ClangIpcServer::changeProjectArgumentsToWrongValues()
{
    RegisterProjectsForCodeCompletionCommand command({ProjectContainer(projectFilePath, {Utf8StringLiteral("-blah")})});

    clangServer.registerProjectsForCodeCompletion(command);
}

const Utf8String ClangIpcServer::unsavedContent(const QString &unsavedFilePath)
{
    QFile unsavedFileContentFile(unsavedFilePath);
    bool isOpen = unsavedFileContentFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!isOpen)
        ADD_FAILURE() << "File with the unsaved content cannot be opened!";

    return Utf8String::fromByteArray(unsavedFileContentFile.readAll());
}

TEST_F(ClangIpcServer, GetCodeCompletion)
{
    CompleteCodeCommand completeCodeCommand(functionTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    CodeCompletion codeCompletion(Utf8StringLiteral("Function"),
                                  Utf8String(),
                                  Utf8String(),
                                  34,
                                  CodeCompletion::FunctionCompletionKind);

    EXPECT_CALL(mockIpcClient, codeCompleted(Property(&CodeCompletedCommand::codeCompletions, Contains(codeCompletion))))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetCodeCompletionDependingOnArgumets)
{
    CompleteCodeCommand completeCodeCommand(variableTestFilePath,
                                            35,
                                            1,
                                            projectFilePath);
    CodeCompletion codeCompletion(Utf8StringLiteral("ArgumentDefinitionVariable"),
                                  Utf8String(),
                                  Utf8String(),
                                  34,
                                  CodeCompletion::VariableCompletionKind);

    EXPECT_CALL(mockIpcClient, codeCompleted(Property(&CodeCompletedCommand::codeCompletions, Contains(codeCompletion))))
        .Times(1);

    changeProjectArguments();
    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetTranslationUnitDoesNotExistForCodeCompletionOnNonExistingTranslationUnit)
{
    CompleteCodeCommand completeCodeCommand(Utf8StringLiteral("dontexists.cpp"),
                                            34,
                                            1,
                                            projectFilePath);
    TranslationUnitDoesNotExistCommand translationUnitDoesNotExistCommand(Utf8StringLiteral("dontexists.cpp"), projectFilePath);

    EXPECT_CALL(mockIpcClient, translationUnitDoesNotExist(translationUnitDoesNotExistCommand))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetTranslationUnitDoesNotExistForCompletingUnregisteredFile)
{
    CompleteCodeCommand completeCodeCommand(parseErrorTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    TranslationUnitDoesNotExistCommand translationUnitDoesNotExistCommand(parseErrorTestFilePath, projectFilePath);

    EXPECT_CALL(mockIpcClient, translationUnitDoesNotExist(translationUnitDoesNotExistCommand))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetCodeCompletionForUnsavedFile)
{
    CompleteCodeCommand completeCodeCommand(functionTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    CodeCompletion codeCompletion(Utf8StringLiteral("Method2"),
                                  Utf8String(),
                                  Utf8String(),
                                  34,
                                  CodeCompletion::FunctionCompletionKind);

    EXPECT_CALL(mockIpcClient, codeCompleted(Property(&CodeCompletedCommand::codeCompletions, Contains(codeCompletion))))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetNoCodeCompletionAfterRemovingUnsavedFile)
{
    clangServer.registerTranslationUnitsForCodeCompletion(RegisterTranslationUnitForCodeCompletionCommand({FileContainer(functionTestFilePath, projectFilePath)}));
    CompleteCodeCommand completeCodeCommand(functionTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    CodeCompletion codeCompletion(Utf8StringLiteral("Method2"),
                                  Utf8String(),
                                  Utf8String(),
                                  34,
                                  CodeCompletion::FunctionCompletionKind);

    EXPECT_CALL(mockIpcClient, codeCompleted(Property(&CodeCompletedCommand::codeCompletions, Not(Contains(codeCompletion)))))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetNewCodeCompletionAfterUpdatingUnsavedFile)
{
    clangServer.registerTranslationUnitsForCodeCompletion(RegisterTranslationUnitForCodeCompletionCommand({FileContainer(functionTestFilePath,
                                                                                                    projectFilePath,
                                                                                                    unsavedContent(updatedUnsavedTestFilePath),
                                                                                                    true)}));
    CompleteCodeCommand completeCodeCommand(functionTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    CodeCompletion codeCompletion(Utf8StringLiteral("Method3"),
                                  Utf8String(),
                                  Utf8String(),
                                  34,
                                  CodeCompletion::FunctionCompletionKind);

    EXPECT_CALL(mockIpcClient, codeCompleted(Property(&CodeCompletedCommand::codeCompletions, Contains(codeCompletion))))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetTranslationUnitDoesNotExistForUnregisterTranslationUnitWithWrongFilePath)
{
    FileContainer fileContainer(Utf8StringLiteral("foo.cpp"), projectFilePath);
    UnregisterTranslationUnitsForCodeCompletionCommand command({fileContainer});
    TranslationUnitDoesNotExistCommand translationUnitDoesNotExistCommand(fileContainer);

    EXPECT_CALL(mockIpcClient, translationUnitDoesNotExist(translationUnitDoesNotExistCommand))
        .Times(1);

    clangServer.unregisterTranslationUnitsForCodeCompletion(command);
}

TEST_F(ClangIpcServer, UnregisterTranslationUnitAndTestFailingCompletion)
{
    FileContainer fileContainer(functionTestFilePath, projectFilePath);
    UnregisterTranslationUnitsForCodeCompletionCommand command({fileContainer});
    clangServer.unregisterTranslationUnitsForCodeCompletion(command);
    CompleteCodeCommand completeCodeCommand(functionTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    TranslationUnitDoesNotExistCommand translationUnitDoesNotExistCommand(fileContainer);

    EXPECT_CALL(mockIpcClient, translationUnitDoesNotExist(translationUnitDoesNotExistCommand))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetProjectDoesNotExistUnregisterProjectInexistingProject)
{
    Utf8String inexistingProjectFilePath = Utf8StringLiteral("projectdoesnotexists.pro");
    UnregisterProjectsForCodeCompletionCommand unregisterProjectsForCodeCompletionCommand({inexistingProjectFilePath});
    ProjectDoesNotExistCommand projectDoesNotExistCommand(inexistingProjectFilePath);

    EXPECT_CALL(mockIpcClient, projectDoesNotExist(projectDoesNotExistCommand))
        .Times(1);

    clangServer.unregisterProjectsForCodeCompletion(unregisterProjectsForCodeCompletionCommand);
}

TEST_F(ClangIpcServer, GetProjectDoesNotExistRegisterTranslationUnitWithInexistingProject)
{
    Utf8String inexistingProjectFilePath = Utf8StringLiteral("projectdoesnotexists.pro");
    RegisterTranslationUnitForCodeCompletionCommand registerFileForCodeCompletionCommand({FileContainer(variableTestFilePath, inexistingProjectFilePath)});
    ProjectDoesNotExistCommand projectDoesNotExistCommand(inexistingProjectFilePath);

    EXPECT_CALL(mockIpcClient, projectDoesNotExist(projectDoesNotExistCommand))
        .Times(1);

    clangServer.registerTranslationUnitsForCodeCompletion(registerFileForCodeCompletionCommand);
}

TEST_F(ClangIpcServer, GetProjectDoesNotExistUnregisterTranslationUnitWithInexistingProject)
{
    Utf8String inexistingProjectFilePath = Utf8StringLiteral("projectdoesnotexists.pro");
    UnregisterTranslationUnitsForCodeCompletionCommand unregisterFileForCodeCompletionCommand({FileContainer(variableTestFilePath, inexistingProjectFilePath)});
    ProjectDoesNotExistCommand projectDoesNotExistCommand(inexistingProjectFilePath);

    EXPECT_CALL(mockIpcClient, projectDoesNotExist(projectDoesNotExistCommand))
        .Times(1);

    clangServer.unregisterTranslationUnitsForCodeCompletion(unregisterFileForCodeCompletionCommand);
}

TEST_F(ClangIpcServer, GetProjectDoesNotExistForCompletingProjectFile)
{
    Utf8String inexistingProjectFilePath = Utf8StringLiteral("projectdoesnotexists.pro");
    CompleteCodeCommand completeCodeCommand(variableTestFilePath,
                                            20,
                                            1,
                                            inexistingProjectFilePath);
    ProjectDoesNotExistCommand projectDoesNotExistCommand(inexistingProjectFilePath);

    EXPECT_CALL(mockIpcClient, projectDoesNotExist(projectDoesNotExistCommand))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, GetProjectDoesNotExistForCompletingUnregisteredFile)
{
    CompleteCodeCommand completeCodeCommand(parseErrorTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    TranslationUnitDoesNotExistCommand translationUnitDoesNotExistCommand(parseErrorTestFilePath, projectFilePath);

    EXPECT_CALL(mockIpcClient, translationUnitDoesNotExist(translationUnitDoesNotExistCommand))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}

TEST_F(ClangIpcServer, TicketNumberIsForwarded)
{
    CompleteCodeCommand completeCodeCommand(functionTestFilePath,
                                            20,
                                            1,
                                            projectFilePath);
    CodeCompletion codeCompletion(Utf8StringLiteral("Function"),
                                  Utf8String(),
                                  Utf8String(),
                                  34,
                                  CodeCompletion::FunctionCompletionKind);

    EXPECT_CALL(mockIpcClient, codeCompleted(Property(&CodeCompletedCommand::ticketNumber, Eq(completeCodeCommand.ticketNumber()))))
        .Times(1);

    clangServer.completeCode(completeCodeCommand);
}
}
