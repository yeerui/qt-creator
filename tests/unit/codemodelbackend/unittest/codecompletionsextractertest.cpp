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

#include <codecompletionsextracter.h>
#include <clangcodecompleteresults.h>
#include <translationunit.h>

using CodeModelBackEnd::CodeCompletionsExtracter;
using CodeModelBackEnd::ClangCodeCompleteResults;
using CodeModelBackEnd::TranslationUnit;
using CodeModelBackEnd::CodeCompletion;

namespace {

using ::testing::PrintToString;

MATCHER_P2(IsCompletion, name, kind,
           std::string(negation ? "isn't" : "is") + " complition of name " + PrintToString(name) +
           " and kind " + PrintToString(kind)
           )
{
    while(arg.next()) {
        if (arg.currentCodeCompletion().text() == name) {
            if (arg.currentCodeCompletion().completionKind() == kind) {
                return true;
            }else if (!arg.peek(name)) {
                *result_listener << "kind is wrong: " << PrintToString(arg.currentCodeCompletion().completionKind());
                return false;
            }
        }
    }

    return false;
}

ClangCodeCompleteResults getResults(const char*filePath, uint line)
{
    TranslationUnit translationUnit(Utf8String::fromUtf8(filePath));

    return ClangCodeCompleteResults(clang_codeCompleteAt(translationUnit.translationUnit(),
                                                                  translationUnit.filePath().constData(),
                                                                  line,
                                                                  1,
                                                                  0,
                                                                  0,
                                                                  CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeCodePatterns));
}

TEST(CodeCompletionExtracter, Function)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Function"), CodeCompletion::FunctionCompletionKind));
}

TEST(CodeCompletionExtracter, TemplateFunction)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("TemplateFunction"), CodeCompletion::TemplateFunctionCompletionKind));
}

TEST(CodeCompletionExtracter, Variable)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_variable.cpp", 4));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Var"), CodeCompletion::VariableCompletionKind));
}


TEST(CodeCompletionExtracter, NonTypeTemplateParameter)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_variable.cpp", 27));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("NonTypeTemplateParameter"), CodeCompletion::VariableCompletionKind));
}


TEST(CodeCompletionExtracter, VariableReference)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_variable.cpp", 12));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Var"), CodeCompletion::VariableCompletionKind));
}

TEST(CodeCompletionExtracter, Parameter)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_variable.cpp", 4));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Parameter"), CodeCompletion::VariableCompletionKind));
}

TEST(CodeCompletionExtracter, Field)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_variable.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Field"), CodeCompletion::VariableCompletionKind));
}

TEST(CodeCompletionExtracter, Class)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Class"), CodeCompletion::ClassCompletionKind));
}

TEST(CodeCompletionExtracter, Struct)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Struct"), CodeCompletion::ClassCompletionKind));
}

TEST(CodeCompletionExtracter, Union)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Union"), CodeCompletion::ClassCompletionKind));
}

TEST(CodeCompletionExtracter, TemplateTypeParameter)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("TemplateTypeParameter"), CodeCompletion::ClassCompletionKind));
}

TEST(CodeCompletionExtracter, TemplateClass)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("TemplateClass"), CodeCompletion::TemplateClassCompletionKind));
}

TEST(CodeCompletionExtracter, TemplateTemplateParameter)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("TemplateTemplateParameter"), CodeCompletion::TemplateClassCompletionKind));
}

TEST(CodeCompletionExtracter, ClassTemplatePartialSpecialization)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_class.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("ClassTemplatePartialSpecialization"), CodeCompletion::TemplateClassCompletionKind));
}

TEST(CodeCompletionExtracter, Namespace)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_namespace.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Namespace"), CodeCompletion::NamespaceCompletionKind));
}

TEST(CodeCompletionExtracter, NamespaceAlias)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_namespace.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("NamespaceAlias"), CodeCompletion::NamespaceCompletionKind));
}

TEST(CodeCompletionExtracter, Enumeration)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_enumeration.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Enumeration"), CodeCompletion::EnumerationCompletionKind));
}

TEST(CodeCompletionExtracter, Enumerator)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_enumeration.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Enumerator"), CodeCompletion::EnumeratorCompletionKind));
}

TEST(CodeCompletionExtracter, Constructor)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_constructor.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Constructor"), CodeCompletion::ConstructorCompletionKind));
}

TEST(CodeCompletionExtracter, Destructor)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_constructor.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("~Constructor"), CodeCompletion::DestructorCompletionKind));
}

TEST(CodeCompletionExtracter, Method)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Method"), CodeCompletion::FunctionCompletionKind));
}

TEST(CodeCompletionExtracter, Slot)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Slot"), CodeCompletion::SlotCompletionKind));
}

TEST(CodeCompletionExtracter, Signal)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("Signal"), CodeCompletion::SignalCompletionKind));
}

TEST(CodeCompletionExtracter, MacroDefinition)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_variable.cpp", 35));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("MacroDefinition"), CodeCompletion::PreProcessorCompletionKind));
}

TEST(CodeCompletionExtracter, FunctionMacro)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 35));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("FunctionMacro"), CodeCompletion::FunctionCompletionKind));
}

TEST(CodeCompletionExtracter, IntKeyword)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("int"), CodeCompletion::KeywordCompletionKind));
}

TEST(CodeCompletionExtracter, SwitchKeyword)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("switch"), CodeCompletion::KeywordCompletionKind));
}

TEST(CodeCompletionExtracter, ClassKeyword)
{
    ClangCodeCompleteResults completeResults(getResults("data/complete_extracter_function.cpp", 20));

    CodeCompletionsExtracter extracter(completeResults.data());

    ASSERT_THAT(extracter, IsCompletion(Utf8StringLiteral("class"), CodeCompletion::KeywordCompletionKind));
}
}
