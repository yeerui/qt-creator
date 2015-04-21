/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
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
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "clangcodecompletion_test.h"

#include <clangcodemodel/clangcompletion.h>
#include <clangcodemodel/constants.h>

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <cpptools/cppcodemodelsettings.h>
#include <cpptools/cpptoolsconstants.h>
#include <cpptools/cpptoolsreuse.h>
#include <cpptools/cpptoolstestcase.h>
#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/codeassist/iassistproposalmodel.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditor.h>

#include <utils/executeondestruction.h>
#include <utils/qtcassert.h>

#include <QDebug>
#include <QtTest>

namespace {

QStringList completionResults(TextEditor::BaseTextEditor *textEditor)
{
    using namespace TextEditor;
    using namespace ClangCodeModel::Internal;

    QStringList results;

    TextEditorWidget *textEditorWidget = qobject_cast<TextEditorWidget *>(textEditor->widget());
    QTC_ASSERT(textEditorWidget, return results);
    AssistInterface *assistInterface = textEditorWidget->createAssistInterface(
                TextEditor::Completion, TextEditor::ExplicitlyInvoked);
    QTC_ASSERT(assistInterface, return results);
    CompletionAssistProvider *assistProvider
            = textEditor->textDocument()->completionAssistProvider();
    QTC_ASSERT(qobject_cast<ClangCompletionAssistProvider *>(assistProvider), return results);
    QTC_ASSERT(assistProvider, return results);
    IAssistProcessor *processor = assistProvider->createProcessor();
    QTC_ASSERT(processor, return results);
    IAssistProposal *assistProposal = processor->perform(assistInterface);
    QTC_ASSERT(assistProposal, return results);
    IAssistProposalModel *assistProposalModel = assistProposal->model();
    QTC_ASSERT(assistProposalModel, return results);

    for (int i = 0, size = assistProposalModel->size(); i < size; ++i)
        results << assistProposalModel->text(i);

    return results;
}

class TestDocument
{
public:
    TestDocument(const QString &fileName)
    {
        QTC_ASSERT(!fileName.isEmpty(), return);
        const QResource resource(QLatin1String(":/unittests/ClangCodeModel/") + fileName);
        QTC_ASSERT(resource.isValid(), return);
        const QByteArray contents = QByteArray(reinterpret_cast<const char*>(resource.data()),
                                               resource.size());
        m_cursorPosition = contents.indexOf(" // COMPLETE HERE");
        if (!contents.isEmpty() && m_cursorPosition > -1)
            m_filePath = m_temporaryDir.createFile(fileName.toUtf8(), contents);
    }

    bool isValid() const { return !m_filePath.isEmpty(); }
    QString filePath() const { return m_filePath; }
    int cursorPosition() const { return m_cursorPosition; }

private:
    QString m_filePath;
    int m_cursorPosition;
    CppTools::Tests::TemporaryDir m_temporaryDir;
};

class OpenEditorAtCursorPosition
{
public:
    OpenEditorAtCursorPosition(const QString &filePath, int cursorPosition);
    ~OpenEditorAtCursorPosition();

    bool succeeded() const { return m_editor; }
    TextEditor::BaseTextEditor *editor() const { return m_editor; }

private:
    TextEditor::BaseTextEditor *m_editor;
};

OpenEditorAtCursorPosition::OpenEditorAtCursorPosition(const QString &filePath, int cursorPosition)
{
    Core::IEditor *coreEditor = Core::EditorManager::openEditor(filePath);
    if ((m_editor = qobject_cast<TextEditor::BaseTextEditor *>(coreEditor)))
        m_editor->setCursorPosition(cursorPosition);
}

OpenEditorAtCursorPosition::~OpenEditorAtCursorPosition()
{
    if (m_editor)
        Core::EditorManager::closeEditor(m_editor);
}

} // anonymous namespace

namespace ClangCodeModel {
namespace Internal {
namespace Tests {

typedef QSharedPointer<CppTools::CppCodeModelSettings> CppCodeModelSettingsPtr;

class ActivateClangModelManagerSupport
{
public:
    ActivateClangModelManagerSupport(CppCodeModelSettingsPtr codeModelSettings);
    ~ActivateClangModelManagerSupport();

private:
    ActivateClangModelManagerSupport();

    CppCodeModelSettingsPtr m_codeModelSettings;
    QHash<QString, QString> m_toRestore;
};

ActivateClangModelManagerSupport::ActivateClangModelManagerSupport(
        CppCodeModelSettingsPtr codeModelSettings)
    : m_codeModelSettings(codeModelSettings)
{
    QTC_CHECK(m_codeModelSettings);
    const QString clangModelManagerSupportId
            = QLatin1String(Constants::CLANG_MODELMANAGERSUPPORT_ID);
    foreach (const QString &mimeType, CppTools::CppCodeModelSettings::supportedMimeTypes()) {
        m_toRestore.insert(mimeType, m_codeModelSettings->modelManagerSupportId(mimeType));
        m_codeModelSettings->setModelManagerSupportId(mimeType, clangModelManagerSupportId);
    }
}

ActivateClangModelManagerSupport::~ActivateClangModelManagerSupport()
{
    QHash<QString, QString>::const_iterator i = m_toRestore.constBegin();
    for (; i != m_toRestore.end(); ++i)
        m_codeModelSettings->setModelManagerSupportId(i.key(), i.value());
}

ClangCodeCompletionTest::ClangCodeCompletionTest()
{
}

ClangCodeCompletionTest::~ClangCodeCompletionTest()
{
}

void ClangCodeCompletionTest::initTestCase()
{
    m_activater.reset(new ActivateClangModelManagerSupport(CppTools::codeModelSettings()));
}

void ClangCodeCompletionTest::testGlobalCompletion()
{
    TestDocument testDocument(QLatin1String("globalCompletion.cpp"));
    QVERIFY(testDocument.isValid());
    OpenEditorAtCursorPosition openEditor(testDocument.filePath(), testDocument.cursorPosition());
    QVERIFY(openEditor.succeeded());

    const QStringList theCompletionResults = completionResults(openEditor.editor());
    QVERIFY(theCompletionResults.contains(QLatin1String("globalVariable")));
    QVERIFY(theCompletionResults.contains(QLatin1String("globalFunction")));
    QVERIFY(theCompletionResults.contains(QLatin1String("GlobalClass")));
}

} // namespace Tests
} // namespace Internal
} // namespace ClangCodeModel
