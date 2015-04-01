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

#include <cmbcodecompletedcommand.h>
#include <cmbcompletecodecommand.h>
#include <cmbechocommand.h>

#include <QBuffer>

namespace {

class MockIpcClient : public CodeModelBackEnd::IpcClientInterface {
 public:
  MOCK_METHOD0(alive,
      void());
  MOCK_METHOD1(echo,
      void(const CodeModelBackEnd::EchoCommand &command));
  MOCK_METHOD1(codeCompleted,
      void(const CodeModelBackEnd::CodeCompletedCommand &command));
};


class ClangInProcess : public ::testing::Test
{
protected:
    ClangInProcess();

    void SetUp();
    void TearDown();


    void scheduleServerCommands();

    QBuffer buffer;
    MockIpcClient mockIpcClient;
    CodeModelBackEnd::ClangIpcServer clangServer;
    CodeModelBackEnd::IpcServerProxy serverProxy;
    CodeModelBackEnd::IpcClientProxy clientProxy;
};


TEST_F(ClangInProcess, GetCodeCompletion)
{
    CodeModelBackEnd::CompleteCodeCommand completeCodeCommand(Utf8StringLiteral("foo.cpp"), 24, 33, Utf8StringLiteral("do what I want"));
    CodeModelBackEnd::CodeCompletedCommand codeCompletedCommand;

    EXPECT_CALL(mockIpcClient, codeCompleted(codeCompletedCommand))
        .Times(1);

    serverProxy.completeCode(completeCodeCommand);

}

ClangInProcess::ClangInProcess()
    : serverProxy(&mockIpcClient, &buffer),
      clientProxy(&clangServer, &buffer)
{
    clangServer.addClient(&mockIpcClient);
}

void ClangInProcess::SetUp()
{
    buffer.open(QIODevice::ReadWrite);
}

void ClangInProcess::TearDown()
{
    scheduleServerCommands();
    buffer.close();
}

void ClangInProcess::scheduleServerCommands()
{
    buffer.seek(0);
    clientProxy.readCommands();
    buffer.buffer().clear();
}
}
