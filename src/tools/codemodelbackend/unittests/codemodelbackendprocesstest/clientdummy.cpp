#include "clientdummy.h"

void ClientDummy::clear()
{
    codeCompletedCommand = CodeModelBackEnd::CodeCompletedCommand ();
}

void ClientDummy::alive()
{
}

void ClientDummy::echo(const CodeModelBackEnd::EchoCommand &command)
{
}

void ClientDummy::codeCompleted(const CodeModelBackEnd::CodeCompletedCommand &command)
{
    codeCompletedCommand = command;

    emit newCodeCompleted();
}
