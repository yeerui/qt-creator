/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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
#ifndef IOSCONFIGURATIONS_H
#define IOSCONFIGURATIONS_H

#include <projectexplorer/abi.h>
#include <utils/fileutils.h>

#include <QObject>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Ios {
namespace Internal {

class IosConfigurations : public QObject
{
    Q_OBJECT

public:
    static QObject *instance();
    static void initialize();
    static bool ignoreAllDevices();
    static void setIgnoreAllDevices(bool ignoreDevices);
    static Utils::FileName developerPath();
    static Utils::FileName lldbPath();

signals:
    void updated();

public slots:
    static void updateAutomaticKitList();

private:
    IosConfigurations(QObject *parent);
    void load();
    void save();
    void updateSimulators();
    static void setDeveloperPath(const Utils::FileName &devPath);

    Utils::FileName m_developerPath;
    Utils::FileName m_lldbPath;
    bool m_ignoreAllDevices;
};

} // namespace Internal
} // namespace Ios

#endif // IOSCONFIGURATIONS_H
