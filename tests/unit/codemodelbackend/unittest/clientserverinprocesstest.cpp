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

#include <QString>
#include <QBuffer>
#include <QVariant>
#include <vector>

#include <cmbendcommand.h>
#include <cmbalivecommand.h>
#include <cmbcommands.h>
#include <cmbechocommand.h>
#include <cmbregisterfilesforcodecompletioncommand.h>
#include <cmbunregisterfilesforcodecompletioncommand.h>
#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>
#include <writecommandblock.h>
#include <readcommandblock.h>

#include <ipcserverproxy.h>
#include <ipcclientproxy.h>

using namespace CodeModelBackEnd;

class MockIpcClient : public IpcClientInterface {
 public:
  MOCK_METHOD0(alive,
      void());
  MOCK_METHOD1(echo,
      void(const EchoCommand &command));
  MOCK_METHOD1(codeCompleted,
      void(const CodeCompletedCommand &command));
};


class MockIpcServer : public IpcServerInterface {
 public:
  MOCK_METHOD0(end,
      void());
  MOCK_METHOD1(registerFilesForCodeCompletion,
      void(const RegisterFilesForCodeCompletionCommand &command));
  MOCK_METHOD1(unregisterFilesForCodeCompletion,
      void(const UnregisterFilesForCodeCompletionCommand &command));
  MOCK_METHOD1(completeCode,
      void(const CompleteCodeCommand &command));
};

class ClientServerInProcess : public ::testing::Test
{
protected:
    ClientServerInProcess();

    void SetUp();
    void TearDown();


    void scheduleServerCommands();
    void scheduleClientCommands();

    QBuffer buffer;
    MockIpcClient mockIpcClient;
    MockIpcServer mockIpcServer;

    CodeModelBackEnd::IpcServerProxy serverProxy;
    CodeModelBackEnd::IpcClientProxy clientProxy;
};

TEST_F(ClientServerInProcess, SendEndCommand)
{
    EXPECT_CALL(mockIpcServer, end())
        .Times(1);

    serverProxy.end();
    scheduleServerCommands();
}

TEST_F(ClientServerInProcess, SendAliveCommand)
{
    EXPECT_CALL(mockIpcClient, alive())
        .Times(1);

    clientProxy.alive();
    scheduleClientCommands();
}

TEST_F(ClientServerInProcess, SendRegisterFilesForCodeCompletionCommand)
{
    CodeModelBackEnd::FileContainer fileContainer(Utf8StringLiteral("foo"));
    QVector<CodeModelBackEnd::FileContainer> fileContainers({fileContainer});
    CodeModelBackEnd::RegisterFilesForCodeCompletionCommand command(fileContainers);

    EXPECT_CALL(mockIpcServer, registerFilesForCodeCompletion(command))
        .Times(1);

    serverProxy.registerFilesForCodeCompletion(command);
    scheduleServerCommands();
}

TEST_F(ClientServerInProcess, SendUnregisterFilesForCodeCompletionCommand)
{
    Utf8StringVector fileNames({Utf8StringLiteral("foo")});
    CodeModelBackEnd::UnregisterFilesForCodeCompletionCommand command(fileNames);

    EXPECT_CALL(mockIpcServer, unregisterFilesForCodeCompletion(command))
        .Times(1);

    serverProxy.unregisterFilesForCodeCompletion(command);
    scheduleServerCommands();
}

TEST_F(ClientServerInProcess, SendCompleteCodeCommand)
{
    CodeModelBackEnd::CompleteCodeCommand command(Utf8StringLiteral("foo.cpp"), 24, 33, Utf8StringLiteral("do what I want"));

    EXPECT_CALL(mockIpcServer, completeCode(command))
        .Times(1);

    serverProxy.completeCode(command);
    scheduleServerCommands();
}

TEST_F(ClientServerInProcess, SendCodeCompletedCommand)
{
    QVector<CodeModelBackEnd::CodeCompletion> codeCompletions({Utf8StringLiteral("newFunction()")});
    CodeModelBackEnd::CodeCompletedCommand command(codeCompletions);

    EXPECT_CALL(mockIpcClient, codeCompleted(command))
        .Times(1);

    clientProxy.codeCompleted(command);
    scheduleClientCommands();
}

ClientServerInProcess::ClientServerInProcess()
    : serverProxy(&mockIpcClient, &buffer),
      clientProxy(&mockIpcServer, &buffer)
{
}

void ClientServerInProcess::SetUp()
{
    buffer.open(QIODevice::ReadWrite);
}

void ClientServerInProcess::TearDown()
{
    buffer.close();
}

void ClientServerInProcess::scheduleServerCommands()
{
    buffer.seek(0);
    clientProxy.readCommands();
    buffer.buffer().clear();
}

void ClientServerInProcess::scheduleClientCommands()
{
    buffer.seek(0);
    serverProxy.readCommands();
    buffer.buffer().clear();
}
