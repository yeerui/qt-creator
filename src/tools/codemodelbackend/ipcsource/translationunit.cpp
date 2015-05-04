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
#include "translationunitfilenotexitsexception.h"
#include "translationunitparseerrorexception.h"
#include "unsavedfiles.h"
#include "project.h"

namespace CodeModelBackEnd {

class TranslationUnitData
{
public:
    TranslationUnitData(const Utf8String &filePath,
                        const UnsavedFiles &unsavedFiles,
                        const Project &project);
    ~TranslationUnitData();

public:
    time_point lastChangeTimePoint;
    Project project;
    Utf8String filePath;
    CXTranslationUnit translationUnit = nullptr;
    CXIndex index = nullptr;
    UnsavedFiles unsavedFiles;
};

TranslationUnitData::TranslationUnitData(const Utf8String &filePath,
                                         const UnsavedFiles &unsavedFiles,
                                         const Project &project)
    : lastChangeTimePoint(std::chrono::high_resolution_clock::now()),
      project(project),
      filePath(filePath),
      unsavedFiles(unsavedFiles)
{
}

TranslationUnitData::~TranslationUnitData()
{
    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
}

TranslationUnit::TranslationUnit(const Utf8String &filePath,
                                 const UnsavedFiles &unsavedFiles,
                                 const Project &project)
    : d(std::make_shared<TranslationUnitData>(filePath, unsavedFiles, project))
{
    checkIfFileExists();
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
        d->index = clang_createIndex(1, 1);

    return d->index;
}

CXTranslationUnit TranslationUnit::cxTranslationUnit() const
{
    checkIfNull();

    removeOutdatedTranslationUnit();

    createTranslationUnitIfNeeded();

    return d->translationUnit;
}

const Utf8String &TranslationUnit::filePath() const
{
    checkIfNull();

    return d->filePath;
}

const Utf8String &TranslationUnit::projectFilePath() const
{
    checkIfNull();

    return d->project.projectFilePath();
}

const time_point &TranslationUnit::lastChangeTimePoint() const
{
    return d->lastChangeTimePoint;
}

void TranslationUnit::checkIfNull() const
{
    if (isNull())
        throw TranslationUnitIsNullException();
}

void TranslationUnit::checkIfFileExists() const
{
    if (!QFileInfo::exists(d->filePath.toString()))
        throw TranslationUnitFileNotExitsException(d->filePath);
}

void TranslationUnit::updateLastChangeTimePoint() const
{
    d->lastChangeTimePoint = std::chrono::high_resolution_clock::now();
}

void TranslationUnit::removeOutdatedTranslationUnit() const
{
    if (d->project.lastChangeTimePoint() > d->lastChangeTimePoint) {
        clang_disposeTranslationUnit(d->translationUnit);
        d->translationUnit = nullptr;
    }
}

void TranslationUnit::createTranslationUnitIfNeeded() const
{
    const auto options = CXTranslationUnit_DetailedPreprocessingRecord
            | CXTranslationUnit_CacheCompletionResults
            | CXTranslationUnit_PrecompiledPreamble
            | CXTranslationUnit_SkipFunctionBodies;

    if (!d->translationUnit) {
        d->translationUnit = CXTranslationUnit();
        CXErrorCode errorCode = clang_parseTranslationUnit2(index(),
                                                            d->filePath.constData(),
                                                            d->project.cxArguments(),
                                                            d->project.argumentCount(),
                                                            d->unsavedFiles.cxUnsavedFiles(),
                                                            d->unsavedFiles.count(),
                                                            options,
                                                            &d->translationUnit);

        checkTranslationUnitErrorCode(errorCode);

        updateLastChangeTimePoint();
    }
}

void TranslationUnit::checkTranslationUnitErrorCode(CXErrorCode errorCode) const
{
    switch (errorCode) {
        case CXError_Success: break;
        default: throw TranslationUnitParseErrorException(d->filePath, d->project.projectFilePath());
    }
}

uint TranslationUnit::unsavedFilesCount() const
{
    return d->unsavedFiles.count();
}

CXUnsavedFile *TranslationUnit::cxUnsavedFiles() const
{
    return d->unsavedFiles.cxUnsavedFiles();
}

TranslationUnit::~TranslationUnit() = default;

TranslationUnit::TranslationUnit(const TranslationUnit &) = default;
TranslationUnit &TranslationUnit::operator =(const TranslationUnit &) = default;

TranslationUnit::TranslationUnit(TranslationUnit &&other)
    : d(std::move(other.d))
{
}

TranslationUnit &TranslationUnit::operator =(TranslationUnit &&other)
{
    d = std::move(other.d);

    return *this;
}

} // namespace CodeModelBackEnd

