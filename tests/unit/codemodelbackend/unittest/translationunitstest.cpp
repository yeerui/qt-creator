/****************************************************************************
**
** Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
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

#include <translationunit.h>
#include <unsavedfiles.h>
#include <utf8string.h>
#include <project.h>
#include <translationunits.h>
#include <filecontainer.h>
#include <projectcontainer.h>
#include <projects.h>
#include <translationunitdoesnotexistsexception.h>
#include <translationunitisnullexception.h>
#include <translationunitfilenotexitsexception.h>
#include <projectdoesnotexistsexception.h>

using CodeModelBackEnd::TranslationUnit;
using CodeModelBackEnd::UnsavedFiles;
using CodeModelBackEnd::Project;

using testing::IsNull;
using testing::NotNull;
using testing::Gt;

namespace {

using ::testing::PrintToString;

MATCHER_P2(IsTranslationUnit, filePath, projectFilePath,
           std::string(negation ? "isn't" : "is") + " translation unit with file path "
           + PrintToString(filePath) + " and project " + PrintToString(projectFilePath)
           )
{
    if (arg.filePath() != filePath) {
        *result_listener << "file path is " + PrintToString(arg.filePath()) + " and not " +  PrintToString(filePath);
        return false;
    }

    if (arg.projectFilePath() != projectFilePath) {
        *result_listener << "file path is " + PrintToString(arg.projectFilePath()) + " and not " +  PrintToString(projectFilePath);
        return false;
    }

    return true;
}

class TranslationUnits : public ::testing::Test
{
protected:
    void SetUp() override;

    CodeModelBackEnd::Projects projects;
    CodeModelBackEnd::UnsavedFiles unsavedFiles;
    CodeModelBackEnd::TranslationUnits translationUnits = CodeModelBackEnd::TranslationUnits(projects, unsavedFiles);
    const Utf8String filePath = Utf8StringLiteral("data/complete_testfile_1.cpp");
    const Utf8String projectFilePath = Utf8StringLiteral("/path/to/projectfile");

};

void TranslationUnits::SetUp()
{
    projects.createOrUpdate({CodeModelBackEnd::ProjectContainer(projectFilePath)});
}


TEST_F(TranslationUnits, ThrowForGettingWithWrongFilePath)
{
    ASSERT_THROW(translationUnits.translationUnit(Utf8StringLiteral("foo.cpp"), projectFilePath),
                 CodeModelBackEnd::TranslationUnitDoesNotExistsException);

}

TEST_F(TranslationUnits, ThrowForGettingWithWrongProjectFilePath)
{
    ASSERT_THROW(translationUnits.translationUnit(filePath, Utf8StringLiteral("foo.pro")),
                 CodeModelBackEnd::ProjectDoesNotExistsException);

}


TEST_F(TranslationUnits, Add)
{
    CodeModelBackEnd::FileContainer fileContainer(filePath, projectFilePath);

    translationUnits.createOrUpdate({fileContainer});

    ASSERT_THAT(translationUnits.translationUnit(filePath, projectFilePath),
                IsTranslationUnit(filePath, projectFilePath));
}

TEST_F(TranslationUnits, ThrowForRemovingWithWrongFilePath)
{
    CodeModelBackEnd::FileContainer fileContainer(Utf8StringLiteral("foo.cpp"), projectFilePath);

    ASSERT_THROW(translationUnits.remove({fileContainer}),
                 CodeModelBackEnd::TranslationUnitDoesNotExistsException);
}

TEST_F(TranslationUnits, ThrowForRemovingWithWrongProjectFilePath)
{
    CodeModelBackEnd::FileContainer fileContainer(filePath, Utf8StringLiteral("foo.pro"));

    ASSERT_THROW(translationUnits.remove({fileContainer}),
                 CodeModelBackEnd::ProjectDoesNotExistsException);
}

TEST_F(TranslationUnits, Remove)
{
    CodeModelBackEnd::FileContainer fileContainer(filePath, projectFilePath);
    translationUnits.createOrUpdate({fileContainer});

    translationUnits.remove({fileContainer});

    ASSERT_THROW(translationUnits.translationUnit(filePath, projectFilePath),
                 CodeModelBackEnd::TranslationUnitDoesNotExistsException);
}

}


