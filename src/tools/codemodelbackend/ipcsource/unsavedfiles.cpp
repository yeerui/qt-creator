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

#include "unsavedfiles.h"


#include <cstring>

namespace CodeModelBackEnd {

void UnsavedFiles::update(const QVector<FileContainer> &fileContainers)
{
    for (const FileContainer &fileContainer : fileContainers)
        updateCXUnsavedFileWithFileContainer(fileContainer);
}

void UnsavedFiles::clear()
{
    for (const CXUnsavedFile &cxUnsavedFile : cxUnsavedFiles_)
        deleteCXUnsavedFile(cxUnsavedFile);

    cxUnsavedFiles_.clear();
}

int UnsavedFiles::unsavedFilesCount() const
{
    return cxUnsavedFiles_.size();
}

CXUnsavedFile *UnsavedFiles::cxUnsavedFiles() const
{
    return cxUnsavedFiles_.data();
}

const std::vector<CXUnsavedFile> &UnsavedFiles::cxUnsavedFileVector() const
{
    return cxUnsavedFiles_;
}

const CXUnsavedFile UnsavedFiles::createCxUnsavedFile(const Utf8String &filePath, const Utf8String &fileContent)
{
    char *cxUnsavedFilePath = new char[filePath.byteSize() + 1];
    char *cxUnsavedFileContent = new char[fileContent.byteSize() + 1];

    std::memcpy(cxUnsavedFilePath, filePath.constData(), filePath.byteSize() + 1);
    std::memcpy(cxUnsavedFileContent, fileContent.constData(), fileContent.byteSize() + 1);

    return CXUnsavedFile { cxUnsavedFilePath, cxUnsavedFileContent, ulong(fileContent.byteSize())};
}

void UnsavedFiles::deleteCXUnsavedFile(const CXUnsavedFile &cxUnsavedFile)
{
    delete [] cxUnsavedFile.Contents;
    delete [] cxUnsavedFile.Filename;
}

void UnsavedFiles::updateCXUnsavedFileWithFileContainer(const FileContainer &fileContainer)
{
    if (fileContainer.hasUnsavedFileContent()) {
        addOrUpdateCXUnsavedFile(fileContainer);
    } else {
        removeCXUnsavedFile(fileContainer);
    }
}

void UnsavedFiles::removeCXUnsavedFile(const FileContainer &fileContainer)
{
    const Utf8String filePath = fileContainer.filePath();
    auto isSameFile = [filePath] (const CXUnsavedFile &cxUnsavedFile) { return filePath == cxUnsavedFile.Filename; };

    cxUnsavedFiles_.erase(std::remove_if(cxUnsavedFiles_.begin(), cxUnsavedFiles_.end(), isSameFile), cxUnsavedFiles_.end());
}

void UnsavedFiles::addOrUpdateCXUnsavedFile(const FileContainer &fileContainer)
{
    const Utf8String filePath = fileContainer.filePath();
    const Utf8String fileContent = fileContainer.unsavedFileContent();
    auto isSameFile = [filePath] (const CXUnsavedFile &cxUnsavedFile) { return filePath == cxUnsavedFile.Filename; };

    auto cxUnsavedFileIterator = std::find_if(cxUnsavedFiles_.begin(), cxUnsavedFiles_.end(), isSameFile);
    if (cxUnsavedFileIterator == cxUnsavedFiles_.end())
        cxUnsavedFiles_.push_back(createCxUnsavedFile(filePath, fileContent));
    else {
        deleteCXUnsavedFile(*cxUnsavedFileIterator);
        *cxUnsavedFileIterator = createCxUnsavedFile(filePath, fileContent);
    }
}


} // namespace CodeModelBackEnd

