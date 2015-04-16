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

#include "translationunit.h"

#include <QFileInfo>

#include <utf8string.h>

#include "codecompleter.h"

#include "translationunitisnullexception.h"
#include "translationunitfilenotexits.h"
#include "translationunithasnounsavedfiles.h"
#include "unsavedfiles.h"

namespace CodeModelBackEnd {

class TranslationUnitData
{
public:
    TranslationUnitData(const Utf8String &filePath, UnsavedFiles *unsavedFiles);
    ~TranslationUnitData();

    Utf8String filePath;
    CXTranslationUnit translationUnit = nullptr;
    CXIndex index = nullptr;
    UnsavedFiles *unsavedFiles;
};

TranslationUnitData::TranslationUnitData(const Utf8String &filePath, UnsavedFiles *unsavedFiles)
    : filePath(filePath),
      unsavedFiles(unsavedFiles)
{
}

TranslationUnitData::~TranslationUnitData()
{
    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
}

TranslationUnit::TranslationUnit(const Utf8String &filePath, UnsavedFiles *unsavedFiles)
    : d(std::make_shared<TranslationUnitData>(filePath, unsavedFiles))
{
    checkIfFileNotExists();
}

bool TranslationUnit::isNull() const
{
    return !d;
}

void TranslationUnit::reset()
{
    d.reset();
}

CXIndex TranslationUnit::index() const
{
    checkIfNull();

    if (!d->index)
        d->index = clang_createIndex(1, 0);

    return d->index;
}

CXTranslationUnit TranslationUnit::cxTranslationUnit() const
{
    checkIfNull();
    checkIfUnsavedFilesExist();

    if (!d->translationUnit)
        d->translationUnit = clang_createTranslationUnitFromSourceFile(index(),
                                                                       d->filePath.constData(),
                                                                       0,
                                                                       0,
                                                                       d->unsavedFiles->unsavedFilesCount(),
                                                                       d->unsavedFiles->cxUnsavedFiles());

    return d->translationUnit;
}

CodeCompleter TranslationUnit::completer() const
{
    return CodeCompleter(*this);
}

const Utf8String TranslationUnit::filePath() const
{
    checkIfNull();

    return d->filePath;
}

void TranslationUnit::checkIfNull() const
{
    if (isNull())
        throw TranslationUnitIsNullException();
}

void TranslationUnit::checkIfFileNotExists() const
{
    if (!QFileInfo::exists(d->filePath.toString()))
        throw TranslationUnitFileNotExits();
}

void TranslationUnit::checkIfUnsavedFilesExist() const
{
    if (!d->unsavedFiles)
        throw TranslationUnitHasNoUnsavedFiles();
}

uint TranslationUnit::unsavedFilesCount() const
{
    checkIfUnsavedFilesExist();
    return d->unsavedFiles->unsavedFilesCount();
}

CXUnsavedFile *TranslationUnit::cxUnsavedFiles() const
{
    checkIfUnsavedFilesExist();
    return d->unsavedFiles->cxUnsavedFiles();
}

TranslationUnit::~TranslationUnit() = default;

TranslationUnit::TranslationUnit(const TranslationUnit &) = default;
TranslationUnit &TranslationUnit::operator =(const TranslationUnit &) = default;

TranslationUnit::TranslationUnit(TranslationUnit &&) = default;
TranslationUnit &TranslationUnit::operator =(TranslationUnit &&) = default;


} // namespace CodeModelBackEnd

