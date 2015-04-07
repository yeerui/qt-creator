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

#include <codecompleter.h>
#include <utf8stringvector.h>

namespace {

using ::testing::ElementsAreArray;
using ::testing::Contains;
using ::testing::AllOf;

TEST(CodeCompletion, CompleteFunc)
{
    using CodeModelBackEnd::CodeCompletion;
    CodeModelBackEnd::CodeCompleter completer;
    completer.setFilePath(Utf8StringLiteral("/home/nik/dev/creator/creator-com-3.4/qtcreator/tests/unit/codemodelbackend/unittest/data/complete_testfile_1.cpp"));
    completer.setLine(49);

    ASSERT_THAT(completer.complete(),
                AllOf(Contains(CodeCompletion(Utf8StringLiteral("functionWithArguments"), Utf8String(), Utf8String(), 0, CodeCompletion::FunctionCompletionKind)),
                      Contains(CodeCompletion(Utf8StringLiteral("function"), Utf8String(), Utf8String(), 0, CodeCompletion::FunctionCompletionKind)),
                      Contains(CodeCompletion(Utf8StringLiteral("otherFunction"), Utf8String(), Utf8String(), 0, CodeCompletion::FunctionCompletionKind)),
                      Contains(CodeCompletion(Utf8StringLiteral("f"), Utf8String(), Utf8String(), 0, CodeCompletion::FunctionCompletionKind))));
}


//TEST(CodeCompletion, CompleteFunc)
//{
//    CodeModelBackEnd::CodeCompleter completer;
//    completer.setFilePath(Utf8StringLiteral("testfile.cpp"));
//    completer.setLine(46);
//    completer.setColumn(4);

//    ASSERT_THAT(completer.complete(),
//                ElementsAreArray({Utf8StringLiteral("function()"),
//                                  Utf8StringLiteral("function2()")}));
//}

//TEST(CodeCompletion, CompleteOther)
//{
//    CodeModelBackEnd::CodeCompleter completer;
//    completer.setFilePath(Utf8StringLiteral("testfile.cpp"));
//    completer.setLine(47);
//    completer.setColumn(5);

//    ASSERT_THAT(completer.complete(),
//                ElementsAreArray({Utf8StringLiteral("otherFunction()")}));
//}
}
