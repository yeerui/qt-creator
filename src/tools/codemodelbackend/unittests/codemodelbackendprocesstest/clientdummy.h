#ifndef CLIENTDUMMY_H
#define CLIENTDUMMY_H

#include "ipcclientinterface.h"


#include <QVariant>
#include <QObject>

#include <QObject>

#include <cmbechocommand.h>
#include <cmbcodecompletedcommand.h>

class ClientDummy : public QObject, public CodeModelBackEnd::IpcClientInterface
{
    Q_OBJECT

public:
    void clear();

    void alive() override;
    void echo(const CodeModelBackEnd::EchoCommand &command) override;
    void codeCompleted(const CodeModelBackEnd::CodeCompletedCommand &command) override;

    CodeModelBackEnd::CodeCompletedCommand codeCompletedCommand;

signals:
    void newCodeCompleted();
};

#endif // CLIENTDUMMY_H
