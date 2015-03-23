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

#include "cmbcompletecodecommand.h"

#include <QDataStream>
#include <QDebug>

namespace CodeModelBackEnd {

CompleteCodeCommand::CompleteCodeCommand(const QByteArray &fileName, quint32 line, quint32 column, const QByteArray &commandLine)
    : fileName_(fileName),
      line_(line),
      column_(column),
      commandLine_(commandLine)
{
}

const QByteArray CompleteCodeCommand::fileName() const
{
    return fileName_;
}

quint32 CompleteCodeCommand::line() const
{
    return line_;
}

quint32 CompleteCodeCommand::column() const
{
    return column_;
}

const QByteArray CompleteCodeCommand::commandLine() const
{
    return commandLine_;
}

QDataStream &operator<<(QDataStream &out, const CompleteCodeCommand &command)
{
    out << command.fileName_;
    out << command.line_;
    out << command.column_;
    out << command.commandLine_;

    return out;
}

QDataStream &operator>>(QDataStream &in, CompleteCodeCommand &command)
{
    in >> command.fileName_;
    in >> command.line_;
    in >> command.column_;
    in >> command.commandLine_;

    return in;
}

bool operator == (const CompleteCodeCommand &first, const CompleteCodeCommand &second)
{
    return first.fileName_ == second.fileName_
            && first.line_ == second.line_
            && first.column_ == second.column_
            && first.commandLine_ == second.commandLine_;
}

bool operator < (const CompleteCodeCommand &first, const CompleteCodeCommand &second)
{
    return first.fileName_ < second.fileName_
            && first.line_ < second.line_
            && first.column_ < second.column_
            && first.commandLine_ < second.commandLine_;
}

QDebug operator <<(QDebug debug, const CompleteCodeCommand &command)
{
    debug.nospace() << "CompleteCodeCommand(";

    debug.nospace() << command.fileName_ << ", ";
    debug.nospace() << command.line_<< ", ";
    debug.nospace() << command.column_<< ", ";
    debug.nospace() << command.commandLine_;

    debug.nospace() << ")";

    return debug;
}

void PrintTo(const CompleteCodeCommand &command, ::std::ostream* os)
{
    *os << "CompleteCodeCommand(";

    *os << command.fileName_.constData() << ", ";
    *os << command.line_ << ", ";
    *os << command.column_ << ", ";
    *os << command.commandLine_.constData();

    *os << ")";
}

} // namespace CodeModelBackEnd

