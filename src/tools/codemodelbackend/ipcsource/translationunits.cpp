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

#include "translationunits.h"

#include <projects.h>
#include <translationunitdoesnotexistsexception.h>

namespace CodeModelBackEnd {

TranslationUnits::TranslationUnits(Projects &projects, UnsavedFiles &unsavedFiles)
    : projects(projects),
      unsavedFiles(unsavedFiles)
{
}

void TranslationUnits::createOrUpdate(const QVector<FileContainer> &fileContainers)
{
    for (const FileContainer &fileContainer : fileContainers)
        createOrUpdateTranslationUnit(fileContainer);
}

void TranslationUnits::remove(const QVector<FileContainer> &fileContainers)
{
    auto lastRemoveBeginIterator = translationUnits.end();

    for (const FileContainer &fileContainer : fileContainers) {
        auto removeBeginIterator = std::remove_if(translationUnits.begin(), lastRemoveBeginIterator, [fileContainer] (const TranslationUnit &translationUnit) {
            return fileContainer.filePath() == translationUnit.filePath() && fileContainer.projectFilePath() == translationUnit.projectFilePath();
        });

        if (removeBeginIterator == lastRemoveBeginIterator)
            throw TranslationUnitDoesNotExistsException(fileContainer);

        lastRemoveBeginIterator = removeBeginIterator;
    }

    translationUnits.erase(lastRemoveBeginIterator, translationUnits.end());
}

const TranslationUnit &TranslationUnits::translationUnit(const Utf8String &filePath, const Utf8String &projectFilePath) const
{
    auto findIterator = findTranslationUnit(filePath, projectFilePath);

    if (findIterator == translationUnits.end())
        throw TranslationUnitDoesNotExistsException(FileContainer(filePath, projectFilePath));

    return *findIterator;
}

void TranslationUnits::createOrUpdateTranslationUnit(const FileContainer &fileContainer)
{
    auto findIterator = findTranslationUnit(fileContainer);
    if (findIterator == translationUnits.end())
        translationUnits.push_back(TranslationUnit(fileContainer.filePath(), unsavedFiles, projects.project(fileContainer.projectFilePath())));
}

const std::vector<TranslationUnit>::iterator TranslationUnits::findTranslationUnit(const FileContainer &fileContainer)
{
    return std::find_if(translationUnits.begin(), translationUnits.end(), [fileContainer] (const TranslationUnit &translationUnit) {
        return fileContainer.filePath() == translationUnit.filePath() && fileContainer.projectFilePath() == translationUnit.projectFilePath();
    });
}

const std::vector<TranslationUnit>::const_iterator TranslationUnits::findTranslationUnit(const Utf8String &filePath, const Utf8String &projectFilePath) const
{
    return std::find_if(translationUnits.begin(), translationUnits.end(), [filePath, projectFilePath] (const TranslationUnit &translationUnit) {
        return filePath == translationUnit.filePath() && projectFilePath == translationUnit.projectFilePath();
    });
}

} // namespace CodeModelBackEnd

