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

#include "codecompleter.h"

#include "clangstring.h"
#include "clangcodecompleteresults.h"
#include "translationunit.h"
#include "codecompletefailedexception.h"

#include <clang-c/Index.h>

#include <QDebug>

#include <qtcassert.h>

namespace CodeModelBackEnd {

QString toString(CXCompletionChunkKind kind)
{
    switch (kind) {
    case CXCompletionChunk_Optional:
        return QLatin1String("Optional");
    case CXCompletionChunk_TypedText:
        return QLatin1String("TypedText");
    case CXCompletionChunk_Text:
        return QLatin1String("Text");
    case CXCompletionChunk_Placeholder:
        return QLatin1String("Placeholder");
    case CXCompletionChunk_Informative:
        return QLatin1String("Informative");
    case CXCompletionChunk_CurrentParameter:
        return QLatin1String("CurrentParameter");
    case CXCompletionChunk_LeftParen:
        return QLatin1String("LeftParen");
    case CXCompletionChunk_RightParen:
        return QLatin1String("RightParen");
    case CXCompletionChunk_LeftBracket:
        return QLatin1String("LeftBracket");
    case CXCompletionChunk_RightBracket:
        return QLatin1String("RightBracket");
    case CXCompletionChunk_LeftBrace:
        return QLatin1String("LeftBrace");
    case CXCompletionChunk_RightBrace:
        return QLatin1String("RightBrace");
    case CXCompletionChunk_LeftAngle:
        return QLatin1String("LeftAngle");
    case CXCompletionChunk_RightAngle:
        return QLatin1String("RightAngle");
    case CXCompletionChunk_Comma:
        return QLatin1String("Comma");
    case CXCompletionChunk_ResultType:
        return QLatin1String("ResultType");
    case CXCompletionChunk_Colon:
        return QLatin1String("Colon");
    case CXCompletionChunk_SemiColon:
        return QLatin1String("SemiColon");
    case CXCompletionChunk_Equal:
        return QLatin1String("Equal");
    case CXCompletionChunk_HorizontalSpace:
        return QLatin1String("HorizontalSpace");
    case CXCompletionChunk_VerticalSpace:
        return QLatin1String("VerticalSpace");
    default:
        return QLatin1String("<UNKNOWN>");
    }
}

CodeCompleter::CodeCompleter(TranslationUnit translationUnit)
    : translationUnit(std::move(translationUnit))
{
}

//void printCompletionString(CXCompletionString completionString, QDebug &deb)
//{
//    uint completionChunkCount = clang_getNumCompletionChunks(completionString);
//    for (uint j = 0; j < completionChunkCount; ++j) {
//        CXString text = clang_getCompletionChunkText(completionString, j);
//        CXCompletionChunkKind chunkKind = clang_getCompletionChunkKind(completionString, j);
//        deb.nospace() << toString(chunkKind) << ":" << clang_getCString(text) << "  ";

//        CXCompletionString subCompletionString = clang_getCompletionChunkCompletionString(completionString, j);
//        if (subCompletionString) {
//            deb << "\tSUBCOMPLETIONSTRING";
//            printCompletionString(subCompletionString, deb);
//        }
//    }
//}

static Utf8String extractTypedText(CXCompletionString completionString)
{
    Utf8String typedText;
    const uint completionChunkCount = clang_getNumCompletionChunks(completionString);
    for (uint chunkIndex = 0; chunkIndex < completionChunkCount; ++chunkIndex) {
        const CXCompletionChunkKind chunkKind = clang_getCompletionChunkKind(completionString, chunkIndex);
        if (chunkKind == CXCompletionChunk_TypedText) {
            const ClangString text(clang_getCompletionChunkText(completionString, chunkIndex));
            typedText.append(text);
        }
    }

    return typedText;
}

static CodeCompletion::Kind convertToCompletionKind(CXCursorKind cursorKind)
{
    if (cursorKind == CXCursor_FunctionDecl)
        return CodeCompletion::FunctionCompletionKind;
    else
        return CodeCompletion::Other;
}

static QVector<CodeCompletion> extractCodeCompletions(CXCodeCompleteResults *completeResults)
{
    QVector<CodeCompletion> completions;
    QTC_ASSERT(completeResults, return completions);

    const uint resultsCount = completeResults->NumResults;
    for (uint i = 0; i < resultsCount; ++i) {
        const CXCompletionResult completionResult = completeResults->Results[i];
        const CXCompletionString completionString = completionResult.CompletionString;

        const CodeCompletion::Kind completionKind = convertToCompletionKind(completionResult.CursorKind);
        const Utf8String typedText = extractTypedText(completionString);

        completions.append(CodeCompletion(typedText,
                                          Utf8String(),
                                          Utf8String(),
                                          0,
                                          completionKind));
    }

    return completions;
}

const QVector<CodeCompletion> CodeCompleter::complete(uint line, uint column) const
{
    QVector<CodeCompletion> completions;

    ClangCodeCompleteResults completeResults(clang_codeCompleteAt(translationUnit.translationUnit(),
                                                                  translationUnit.filePath().constData(),
                                                                  line,
                                                                  column,
                                                                  0,
                                                                  0,
                                                                  0));

    checkCodeCompleteResult(completeResults.data());

    return extractCodeCompletions(completeResults.data());
}

const Utf8String CodeCompleter::filePath() const
{
    return translationUnit.filePath();
}

void CodeCompleter::checkCodeCompleteResult(CXCodeCompleteResults *completeResults)
{
    if (!completeResults)
        throw CodeCompleteFailedException();
}

} // namespace CodeModelBackEnd

