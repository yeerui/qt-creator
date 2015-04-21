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
#include "gmock/gmock-generated-matchers.h"

#include <clang-c/Index.h>

#include <codecompletionsextractor.h>
#include <clangcodecompleteresults.h>
#include <filecontainer.h>
#include <translationunit.h>
#include <unsavedfiles.h>
#include <project.h>
#include <utf8stringvector.h>

#include <QFile>

using CodeModelBackEnd::CodeCompletionsExtractor;
using CodeModelBackEnd::ClangCodeCompleteResults;
using CodeModelBackEnd::TranslationUnit;
using CodeModelBackEnd::CodeCompletion;
using CodeModelBackEnd::UnsavedFiles;

namespace {

using ::testing::PrintToString;
using ::testing::Not;

MATCHER_P3(IsCompletion, name, kind,  availability,
           std::string(negation ? "isn't" : "is") + " completion of name " + PrintToString(name) +
           ", kind " + PrintToString(kind))
{
    while(arg.next()) {
        if (arg.currentCodeCompletion().text() == name) {
            if (arg.currentCodeCompletion().completionKind() == kind) {
                if (arg.currentCodeCompletion().availability() == availability) {
                    return true;
                } else if (!arg.peek(name)) {
                    *result_listener << "availability is " << PrintToString(arg.currentCodeCompletion().availability()) << " and not " << PrintToString(availability);
                    return false;
                }
            } else if (!arg.peek(name)) {
                *result_listener << "kind is " << PrintToString(arg.currentCodeCompletion().completionKind()) << " and not " << PrintToString(kind);
                return false;
            }
        }
    }

    return false;
}

ClangCodeCompleteResults getResultsWithUnsavedFile(const char *filePath, const char *unsavedFilePath, uint line)
{
    QFile unsavedFileContentFile(QString::fromUtf8(unsavedFilePath));
    bool isOpen = unsavedFileContentFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!isOpen)
         ADD_FAILURE() << "File with the unsaved content cannot be opened!";

    const Utf8String unsavedFileContent = Utf8String::fromByteArray(unsavedFileContentFile.readAll());
    const CodeModelBackEnd::FileContainer unsavedDataFileContainer(Utf8String::fromUtf8(filePath), unsavedFileContent, true);

    CodeModelBackEnd::UnsavedFiles unsavedFiles;
    unsavedFiles.update({unsavedDataFileContainer});

    CodeModelBackEnd::Project project(Utf8StringLiteral("/path/to/projectfile"));

    TranslationUnit translationUnit(Utf8String::fromUtf8(filePath), unsavedFiles, project);

    return ClangCodeCompleteResults(clang_codeCompleteAt(translationUnit.cxTranslationUnit(),
                                                         translationUnit.filePath().constData(),
                                                         line,
                                                         1,
                                                         translationUnit.cxUnsavedFiles(),
                                                         translationUnit.unsavedFilesCount(),
                                                         CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeCodePatterns));
}

ClangCodeCompleteResults getResults(const TranslationUnit &translationUnit, uint line, uint column = 1)
{
    return ClangCodeCompleteResults(clang_codeCompleteAt(translationUnit.cxTranslationUnit(),
                                                         translationUnit.filePath().constData(),
                                                         line,
                                                         column,
                                                         translationUnit.cxUnsavedFiles(),
                                                         translationUnit.unsavedFilesCount(),
                                                         CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeCodePatterns));
}

class CodeCompletionExtractor : public ::testing::Test
{
protected:
    CodeModelBackEnd::Project project = Utf8StringLiteral("/path/to/projectfile");
    CodeModelBackEnd::UnsavedFiles unsavedFiles;
    TranslationUnit functionTranslationUnit = TranslationUnit(Utf8StringLiteral("data/complete_extractor_function.cpp"), unsavedFiles, project);
    TranslationUnit variableTranslationUnit = TranslationUnit(Utf8StringLiteral("data/complete_extractor_variable.cpp"), unsavedFiles, project);
    TranslationUnit classTranslationUnit = TranslationUnit(Utf8StringLiteral("data/complete_extractor_class.cpp"), unsavedFiles, project);
    TranslationUnit namespaceTranslationUnit = TranslationUnit(Utf8StringLiteral("data/complete_extractor_namespace.cpp"), unsavedFiles, project);
    TranslationUnit enumerationTranslationUnit = TranslationUnit(Utf8StringLiteral("data/complete_extractor_enumeration.cpp"), unsavedFiles, project);
    TranslationUnit constructorTranslationUnit = TranslationUnit(Utf8StringLiteral("data/complete_extractor_constructor.cpp"), unsavedFiles, project);
};


TEST_F(CodeCompletionExtractor, Function)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Function"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, TemplateFunction)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("TemplateFunction"),
                                        CodeCompletion::TemplateFunctionCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Variable)
{
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 4));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Var"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available));
}


TEST_F(CodeCompletionExtractor, NonTypeTemplateParameter)
{
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 25, 19));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("NonTypeTemplateParameter"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available));
}


TEST_F(CodeCompletionExtractor, VariableReference)
{
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 12));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Var"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Parameter)
{
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 4));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Parameter"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Field)
{
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Field"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Class)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Class"),
                                        CodeCompletion::ClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Struct)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Struct"),
                                        CodeCompletion::ClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Union)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Union"),
                                        CodeCompletion::ClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, TemplateTypeParameter)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("TemplateTypeParameter"),
                                        CodeCompletion::ClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, TemplateClass)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("TemplateClass"),
                                        CodeCompletion::TemplateClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, TemplateTemplateParameter)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("TemplateTemplateParameter"),
                                        CodeCompletion::TemplateClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, ClassTemplatePartialSpecialization)
{
    ClangCodeCompleteResults completeResults(getResults(classTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("ClassTemplatePartialSpecialization"),
                                        CodeCompletion::TemplateClassCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Namespace)
{
    ClangCodeCompleteResults completeResults(getResults(namespaceTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Namespace"),
                                        CodeCompletion::NamespaceCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, NamespaceAlias)
{
    ClangCodeCompleteResults completeResults(getResults(namespaceTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("NamespaceAlias"),
                                        CodeCompletion::NamespaceCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Enumeration)
{
    ClangCodeCompleteResults completeResults(getResults(enumerationTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Enumeration"),
                                        CodeCompletion::EnumerationCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Enumerator)
{
    ClangCodeCompleteResults completeResults(getResults(enumerationTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Enumerator"),
                                        CodeCompletion::EnumeratorCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Constructor)
{
    ClangCodeCompleteResults completeResults(getResults(constructorTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Constructor"),
                                        CodeCompletion::ConstructorCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Destructor)
{
    ClangCodeCompleteResults completeResults(getResults(constructorTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("~Constructor"),
                                        CodeCompletion::DestructorCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Method)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Method"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::Available));
    ASSERT_FALSE(extractor.currentCodeCompletion().hasParameters());
}

TEST_F(CodeCompletionExtractor, MethodWithParameters)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("MethodWithParameters"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::Available));
    ASSERT_TRUE(extractor.currentCodeCompletion().hasParameters());
}

TEST_F(CodeCompletionExtractor, Slot)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Slot"),
                                        CodeCompletion::SlotCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, Signal)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Signal"),
                                        CodeCompletion::SignalCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, MacroDefinition)
{
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 35));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("MacroDefinition"),
                                        CodeCompletion::PreProcessorCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, FunctionMacro)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("FunctionMacro"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, IntKeyword)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("int"),
                                        CodeCompletion::KeywordCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, SwitchKeyword)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("switch"),
                                        CodeCompletion::KeywordCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, ClassKeyword)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("class"),
                                        CodeCompletion::KeywordCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, DeprecatedFunction)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("DeprecatedFunction"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::Deprecated));
}

TEST_F(CodeCompletionExtractor, NotAccessibleFunction)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));
    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("NotAccessibleFunction"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::NotAccessible));
}

TEST_F(CodeCompletionExtractor, NotAvailableFunction)
{
    ClangCodeCompleteResults completeResults(getResults(functionTranslationUnit, 20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("NotAvailableFunction"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::NotAvailable));
}

TEST_F(CodeCompletionExtractor, UnsavedFile)
{
    ClangCodeCompleteResults completeResults(getResultsWithUnsavedFile("data/complete_extractor_function.cpp",
                                                                       "data/complete_extractor_function_unsaved.cpp",
                                                                       20));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("Method2"),
                                        CodeCompletion::FunctionCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, ArgumentDefinition)
{
    project.setArguments({Utf8StringLiteral("-DArgumentDefinition")});
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 35));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, IsCompletion(Utf8StringLiteral("ArgumentDefinitionVariable"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available));
}

TEST_F(CodeCompletionExtractor, NoArgumentDefinition)
{
    project.setArguments(Utf8StringVector());
    ClangCodeCompleteResults completeResults(getResults(variableTranslationUnit, 35));

    CodeCompletionsExtractor extractor(completeResults.data());

    ASSERT_THAT(extractor, Not(IsCompletion(Utf8StringLiteral("ArgumentDefinitionVariable"),
                                        CodeCompletion::VariableCompletionKind,
                                        CodeCompletion::Available)));
}
}
