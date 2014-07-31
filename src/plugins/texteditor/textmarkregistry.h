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
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef BASETEXTMARKREGISTRY_H
#define BASETEXTMARKREGISTRY_H

#include <utils/fileutils.h>

#include <QObject>
#include <QHash>
#include <QSet>

namespace Core {
class IEditor;
class IDocument;
}

namespace TextEditor {
class TextMark;
namespace Internal {

class TextMarkRegistry : public QObject
{
    Q_OBJECT
public:
    TextMarkRegistry(QObject *parent);

    void add(TextMark *mark);
    bool remove(TextMark *mark);
private slots:
    void editorOpened(Core::IEditor *editor);
    void documentRenamed(Core::IDocument *document, const QString &oldName, const QString &newName);
    void allDocumentsRenamed(const QString &oldName, const QString &newName);
private:
    QHash<Utils::FileName, QSet<TextMark *> > m_marks;
};

} // namespace Internal
} // namespace TextEditor

#endif // BASETEXTMARKREGISTRY_H