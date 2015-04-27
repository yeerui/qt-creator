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

#ifndef CODEMODELBACKEND_PROJECTCONTAINER_H
#define CODEMODELBACKEND_PROJECTCONTAINER_H

#include <QMetaType>

#include <utf8stringvector.h>

#include <codemodelbackendipc_global.h>

namespace CodeModelBackEnd {

class CMBIPC_EXPORT ProjectContainer
{
    friend QDataStream &operator<<(QDataStream &out, const ProjectContainer &container);
    friend QDataStream &operator>>(QDataStream &in, ProjectContainer &container);
    friend bool operator == (const ProjectContainer &first, const ProjectContainer &second);
    friend bool operator < (const ProjectContainer &first, const ProjectContainer &second);
public:
    ProjectContainer() = default;
    ProjectContainer(const Utf8String &filePath,
                     const Utf8StringVector &arguments = Utf8StringVector());

    const Utf8String &filePath() const;
    const Utf8StringVector &arguments() const;

private:
    Utf8String filePath_;
    Utf8StringVector arguments_;
};

CMBIPC_EXPORT QDataStream &operator<<(QDataStream &out, const ProjectContainer &container);
CMBIPC_EXPORT QDataStream &operator>>(QDataStream &in, ProjectContainer &container);
CMBIPC_EXPORT bool operator == (const ProjectContainer &first, const ProjectContainer &second);
CMBIPC_EXPORT bool operator < (const ProjectContainer &first, const ProjectContainer &second);

QDebug operator <<(QDebug debug, const ProjectContainer &container);
void PrintTo(const ProjectContainer &container, ::std::ostream* os);

} // namespace CodeModelBackEnd

Q_DECLARE_METATYPE(CodeModelBackEnd::ProjectContainer)

#endif // CODEMODELBACKEND_PROJECTCONTAINER_H
