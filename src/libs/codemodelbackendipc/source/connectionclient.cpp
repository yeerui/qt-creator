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

#include "connectionclient.h"

#include <QProcess>
#include <QThread>
#include <QCoreApplication>

#include "cmbregisterfilesforcodecompletioncommand.h"
#include "cmbunregisterfilesforcodecompletioncommand.h"
#include "cmbcompletecodecommand.h"

namespace CodeModelBackEnd {

ConnectionClient::ConnectionClient(IpcClientInterface *client)
    : serverProxy_(client, &localSocket),
      isInConnectedMode(false)
{
    processAliveTimer.setInterval(10000);

    connect(&processAliveTimer, &QTimer::timeout, this, &ConnectionClient::restartProcess);
}

ConnectionClient::~ConnectionClient()
{
    finishProcess();
}

bool ConnectionClient::connectToServer()
{localSocket.connectToServer(QStringLiteral("blahxxxxxy"));
    isInConnectedMode = true;
    bool isConnected = localSocket.waitForConnected();
    if (!isConnected) {
        startProcess();
        resetProcessAliveTimer();
        isConnected = retryToConnectToServer();
    }

    return isConnected;
}

bool ConnectionClient::disconnectFromServer()
{
    localSocket.disconnectFromServer();
    isInConnectedMode = false;
    if (localSocket.state() != QLocalSocket::UnconnectedState)
        return localSocket.waitForDisconnected();

    return true;
}

bool ConnectionClient::isConnected() const
{
    return localSocket.state() == QLocalSocket::ConnectedState;
}

void ConnectionClient::sendEndCommand()
{
    serverProxy_.end();
    localSocket.flush();
}

void ConnectionClient::sendRegisterFilesForCodeCompletionCommand(const QVector<FileContainer> &fileContainers)
{
    serverProxy_.registerFilesForCodeCompletion(RegisterFilesForCodeCompletionCommand(fileContainers));
}

void ConnectionClient::sendUnregisterFilesForCodeCompletionCommand(const QVector<FileContainer> &fileContainers)
{
    serverProxy_.unregisterFilesForCodeCompletion(UnregisterFilesForCodeCompletionCommand(fileContainers));
}

void ConnectionClient::sendCompleteCodeCommand(const Utf8String &fileName, quint32 line, quint32 column, const Utf8String &projectFilePath)
{
    serverProxy_.completeCode(CompleteCodeCommand(fileName, line, column, projectFilePath));
}

void ConnectionClient::resetProcessAliveTimer()
{
    processAliveTimer.start();
}

void ConnectionClient::setProcessAliveTimerInterval(int processTimerInterval)
{
    processAliveTimer.setInterval(processTimerInterval);
}

void ConnectionClient::startProcess()
{
    if (!isProcessIsRunning()) {
        process()->setProcessChannelMode(QProcess::ForwardedErrorChannel);
        process()->setReadChannel(QProcess::StandardOutput);
        process()->start(processPath());
        process()->waitForStarted();
        processAliveTimer.start();
    }
}

void ConnectionClient::restartProcess()
{
    finishProcess();
    startProcess();

    connectToServer();

    emit processRestarted();
}

bool ConnectionClient::retryToConnectToServer()
{
    for (int counter = 0; counter < 1000; counter++) {
        localSocket.connectToServer(QStringLiteral("blahxxxxxy"));
        bool isConnected = localSocket.waitForConnected(20);

        if (isConnected)
            return isConnected;
        else
            QThread::msleep(30);
    }

    qDebug() << "Cannot connect:" <<localSocket.errorString();

    return false;
}

void ConnectionClient::endProcess()
{
    if (isProcessIsRunning()) {
        sendEndCommand();
        process()->waitForFinished();
    }
}

void ConnectionClient::terminateProcess()
{
#ifndef Q_OS_WIN32
    if (isProcessIsRunning()) {
        process()->terminate();
        process()->waitForFinished();
    }
#endif
}

void ConnectionClient::killProcess()
{
    if (isProcessIsRunning()) {
        process()->kill();
        process()->waitForFinished();
    }
}

void ConnectionClient::finishProcess()
{
    processAliveTimer.stop();

    endProcess();
    disconnectFromServer();
    terminateProcess();
    killProcess();

    process_.reset();

    serverProxy_.resetCounter();
}

bool ConnectionClient::waitForEcho()
{
    return localSocket.waitForReadyRead();
}

IpcServerProxy &ConnectionClient::serverProxy()
{
    return serverProxy_;
}

bool ConnectionClient::isProcessIsRunning() const
{
    return process_ && process_->state() == QProcess::Running;
}

QProcess *ConnectionClient::process() const
{
    if (!process_)
        process_.reset(new QProcess);

    return process_.get();
}

const QString ConnectionClient::processPath() const
{
    return processPath_;
}

void ConnectionClient::setProcessPath(const QString &processPath)
{
    processPath_ = processPath;
}

} // namespace CodeModelBackEnd

