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

#ifndef CODEMODELBACKEND_CODECOMPLETION_H
#define CODEMODELBACKEND_CODECOMPLETION_H

#include <QMetaType>
#include <QByteArray>

#include "codemodelbackendipc_global.h"

namespace CodeModelBackEnd {

class CMBIPC_EXPORT CodeCompletion
{
    friend QDataStream &operator<<(QDataStream &out, const CodeCompletion &command);
    friend QDataStream &operator>>(QDataStream &in, CodeCompletion &command);
    friend bool operator == (const CodeCompletion &first, const CodeCompletion &second);
    friend bool operator < (const CodeCompletion &first, const CodeCompletion &second);
    friend QDebug operator <<(QDebug debug, const CodeCompletion &command);
    friend void PrintTo(const CodeCompletion &command, ::std::ostream* os);

public:
    enum Kind {
        Other = 0,
        FunctionCompletionKind,
        ConstructorCompletionKind,
        DestructorCompletionKind,
        VariableCompletionKind,
        ClassCompletionKind,
        EnumCompletionKind,
        EnumeratorCompletionKind,
        NamespaceCompletionKind,
        PreProcessorCompletionKind,
        SignalCompletionKind,
        SlotCompletionKind,
        ObjCMessageCompletionKind,
        KeywordCompletionKind,
        ClangSnippetKind
    };

    enum Availability {
        Available,
        Deprecated,
        NotAvailable,
        NotAccessible
    };

public:
    CodeCompletion() = default;
    CodeCompletion(const QByteArray &text,
                   const QByteArray &hint = QByteArray(),
                   const QByteArray &snippet = QByteArray(),
                   quint32 priority = 0,
                   Kind completionKind = Other,
                   Availability availability = Available,
                   bool hasParameters = false);

    const QByteArray &text() const;
    const QByteArray &hint() const;
    const QByteArray &snippet() const;
    Kind completionKind() const;
    Availability availability() const;
    bool hasParameters() const;
    quint32 priority() const;

private:
    QByteArray text_;
    QByteArray hint_;
    QByteArray snippet_;
    quint32 priority_;
    union {
        Kind completionKind_;
        qint32 completionKindAsInt;
    };
    union {
        Availability availability_;
        qint32 availabilityAsInt;
    };
    bool hasParameters_;
};

QDataStream &operator<<(QDataStream &out, const CodeCompletion &command);
QDataStream &operator>>(QDataStream &in, CodeCompletion &command);
bool operator == (const CodeCompletion &first, const CodeCompletion &second);
bool operator < (const CodeCompletion &first, const CodeCompletion &second);

QDebug operator <<(QDebug debug, const CodeCompletion &command);
void PrintTo(const CodeCompletion &command, ::std::ostream* os);

} // namespace CodeModelBackEnd

Q_DECLARE_METATYPE(CodeModelBackEnd::CodeCompletion)

#endif // CODEMODELBACKEND_CODECOMPLETION_H
