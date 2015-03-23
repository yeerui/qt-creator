#include <QString>
#include <QtTest>

#include <connectionclient.h>
#include <clientdummy.h>
#include <cmbcommands.h>
#include <filecontainer.h>

#include <cmbcompletecodecommand.h>
#include <cmbcodecompletedcommand.h>

static const char fooHeaderFile[] = TEST_BASE_DIRECTORY "/foo.h";

class CodeModelBackendProcessTest : public QObject
{
    Q_OBJECT

public:
    CodeModelBackendProcessTest();

private Q_SLOTS:
    void init();
    void cleanup();
    void completeCode();

private:
    void registerFiles();

private:
    ClientDummy clientDummy;
    CodeModelBackEnd::ConnectionClient client;
};

CodeModelBackendProcessTest::CodeModelBackendProcessTest()
    : client(&clientDummy)
{
    CodeModelBackEnd::Commands::registerCommands();
    client.setProcessPath(QLatin1String(CODEMODELBACKENDPROCESSPATH));
}

void CodeModelBackendProcessTest::init()
{
    client.startProcess();
    client.connectToServer();

}

void CodeModelBackendProcessTest::cleanup()
{
    client.disconnectFromServer();
    client.sendEndCommand();
}

void CodeModelBackendProcessTest::completeCode()
{
    QSignalSpy clientSpy(&clientDummy, &ClientDummy::newCodeCompleted);
    registerFiles();

    client.sendCompleteCodeCommand(fooHeaderFile, 36, 0, "");

    QVERIFY(clientSpy.wait(3000));
}

void CodeModelBackendProcessTest::registerFiles()
{
    CodeModelBackEnd::FileContainer fileContainer(fooHeaderFile);
    QVector<CodeModelBackEnd::FileContainer> fileContainers({fileContainer});

    client.sendRegisterFilesForCodeCompletionCommand(fileContainers);
}

QTEST_MAIN(CodeModelBackendProcessTest)

#include "tst_codemodelbackendprocess.moc"
