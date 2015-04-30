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

#include "projectcontainer.h"

#include <QtDebug>

#include <QDataStream>

namespace CodeModelBackEnd {

ProjectContainer::ProjectContainer(const Utf8String &fileName,
                                   const Utf8StringVector &arguments)
    : filePath_(fileName),
      arguments_(arguments)
{
}

const Utf8String &ProjectContainer::filePath() const
{
    return filePath_;
}

const Utf8StringVector &ProjectContainer::arguments() const
{
    return arguments_;
}


QDataStream &operator<<(QDataStream &out, const ProjectContainer &container)
{
    out << container.filePath_;
    out << container.arguments_;

    return out;
}

QDataStream &operator>>(QDataStream &in, ProjectContainer &container)
{
    in >> container.filePath_;
    in >> container.arguments_;

    return in;
}

bool operator == (const ProjectContainer &first, const ProjectContainer &second)
{
    return first.filePath_ == second.filePath_;
}

bool operator < (const ProjectContainer &first, const ProjectContainer &second)
{
    return first.filePath_ < second.filePath_;
}

QDebug operator <<(QDebug debug, const ProjectContainer &container)
{
    debug.nospace() << "ProjectContainer("
                    << container.filePath()
                    << ")";

    return debug;
}

void PrintTo(const ProjectContainer &container, ::std::ostream* os)
{
    *os << "ProjectContainer("
        << container.filePath().constData()
        << ")";
}


} // namespace CodeModelBackEnd

