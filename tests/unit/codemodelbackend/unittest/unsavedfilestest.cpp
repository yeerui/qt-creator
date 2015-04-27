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
#include "gmock/gmock.h"

#include <unsavedfiles.h>
#include <filecontainer.h>

#include <QVector>

using CodeModelBackEnd::UnsavedFiles;
using CodeModelBackEnd::FileContainer;

using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Gt;

namespace {

bool operator ==(const CodeModelBackEnd::FileContainer &fileContainer, const CXUnsavedFile &cxUnsavedFile)
{
    return fileContainer.filePath() == Utf8String::fromUtf8(cxUnsavedFile.Filename)
            && fileContainer.unsavedFileContent() == Utf8String(cxUnsavedFile.Contents, cxUnsavedFile.Length);
}

bool fileContainersContainsItemMatchingToCxUnsavedFile(const QVector<FileContainer> &fileContainers, const CXUnsavedFile &cxUnsavedFile)
{
    for (const FileContainer &fileContainer : fileContainers)
        if (fileContainer == cxUnsavedFile)
            return true;

    return false;
}

MATCHER_P(HasUnsavedFiles, fileContainers, "")
{
    CodeModelBackEnd::UnsavedFiles unsavedFiles = arg;
    if (unsavedFiles.count() != fileContainers.size()) {
        *result_listener << "unsaved count is " << unsavedFiles.count() << " and not " << fileContainers.size();
        return false;
    }

    for (const CXUnsavedFile &cxUnsavedFile : unsavedFiles.cxUnsavedFileVector()) {
        if (!fileContainersContainsItemMatchingToCxUnsavedFile(fileContainers, cxUnsavedFile))
            return false;
    }

    return true;
}

class UnsavedFiles : public ::testing::Test
{
protected:
    void TearDown() override;

protected:
    ::UnsavedFiles unsavedFiles;
};

void UnsavedFiles::TearDown()
{
    unsavedFiles.clear();
}

TEST_F(UnsavedFiles, DoNothingForUpdateIfFileHasNoUnsavedContent)
{
    QVector<FileContainer> fileContainers({FileContainer(Utf8StringLiteral("file.cpp"))});

    unsavedFiles.update(fileContainers);

    ASSERT_THAT(unsavedFiles, HasUnsavedFiles(QVector<FileContainer>()));
}

TEST_F(UnsavedFiles, AddUnsavedFileForUpdateWithUnsavedContent)
{
    QVector<FileContainer> fileContainers({FileContainer(Utf8StringLiteral("file.cpp")),
                                           FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo"), true)});
    unsavedFiles.update(fileContainers);

    ASSERT_THAT(unsavedFiles, HasUnsavedFiles(QVector<FileContainer>({FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo"), true)})));
}

TEST_F(UnsavedFiles, RemoveUnsavedFileForUpdateWithUnsavedContent)
{
    QVector<FileContainer> fileContainers({FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo"), true),
                                           FileContainer(Utf8StringLiteral("file.cpp"))});

    unsavedFiles.update(fileContainers);

    ASSERT_THAT(unsavedFiles, HasUnsavedFiles(QVector<FileContainer>()));
}

TEST_F(UnsavedFiles, ExchangeUnsavedFileForUpdateWithUnsavedContent)
{
    QVector<FileContainer> fileContainers({FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo"), true),
                                           FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo2"), true)});
    unsavedFiles.update(fileContainers);

    ASSERT_THAT(unsavedFiles, HasUnsavedFiles(QVector<FileContainer>({FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo2"), true)})));
}

TEST_F(UnsavedFiles, TimeStampIsUpdatedAsUnsavedFilesChanged)
{
    QVector<FileContainer> fileContainers({FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo"), true),
                                           FileContainer(Utf8StringLiteral("file.cpp"), Utf8StringLiteral("foo2"), true)});
    auto lastChangeTimePoint = unsavedFiles.lastChangeTimePoint();

    unsavedFiles.update(fileContainers);

    ASSERT_THAT(unsavedFiles.lastChangeTimePoint(), Gt(lastChangeTimePoint));

}

}



