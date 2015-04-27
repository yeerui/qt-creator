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
#include <translationunitdonotexistsexception.h>

#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>
#include <cmbechocommand.h>
#include <cmbregisterfilesforcodecompletioncommand.h>
#include <cmbunregisterfilesforcodecompletioncommand.h>
#include <cmbregisterprojectsforcodecompletioncommand.h>

#include <QBuffer>


using testing::Property;
using testing::Contains;

namespace {

using CodeModelBackEnd::RegisterFilesForCodeCompletionCommand;
using CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand;
using CodeModelBackEnd::RegisterProjectsForCodeCompletionCommand;
using CodeModelBackEnd::CompleteCodeCommand;
using CodeModelBackEnd::CodeCompletedCommand;
using CodeModelBackEnd::CodeCompletion;
using CodeModelBackEnd::FileContainer;
using CodeModelBackEnd::ProjectContainer;

class MockIpcClient : public CodeModelBackEnd::IpcClientInterface {
 public:
  MOCK_METHOD0(alive,
      void());
  MOCK_METHOD1(echo,
      void(const CodeModelBackEnd::EchoCommand &command));
  MOCK_METHOD1(codeCompleted,
      void(const CodeModelBackEnd::CodeCompletedCommand &command));
};


class ClangIpcServer : public ::testing::Test
{
protected:
    void SetUp() override;

    void registerFiles();
    void registerProject();
    void changeProjectArguments();

protected:
    MockIpcClient mockIpcClient;
    CodeModelBackEnd::ClangIpcServer clangServer;
    const Utf8String projectFilePath = Utf8StringLiteral("pathToProject.pro");
    const Utf8String functionTestFilePath = Utf8StringLiteral("data/complete_extractor_function.cpp");
    const Utf8String variableTestFilePath = Utf8StringLiteral("data/complete_extractor_variable.cpp");
};


void ClangIpcServer::SetUp()
{
    clangServer.addClient(&mockIpcClient);
    registerProject();
    registerFiles();
}

void ClangIpcServer::registerFiles()
{
    RegisterFilesForCodeCompletionCommand command({FileContainer(functionTestFilePath, projectFilePath),
                                                   FileContainer(variableTestFilePath, projectFilePath)});

    clangServer.registerFilesForCodeCompletion(command);
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

TEST_F(ClangIpcServer, ThrowExceptionForNonExistingTranslationUnit)
{
    CompleteCodeCommand completeCodeCommand(Utf8StringLiteral("dontexists.cpp"),
                                            34,
                                            1,
                                            Utf8String());

    ASSERT_THROW(clangServer.completeCode(completeCodeCommand), CodeModelBackEnd::TranslationUnitDoNotExistsException);
}

TEST_F(ClangIpcServer, RegisterProject)
{

}


}
