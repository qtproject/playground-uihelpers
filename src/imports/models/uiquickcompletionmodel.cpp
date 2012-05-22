/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT).
** Contact: http://www.qt-project.org/
**
** This file is part of the UiHelpers playground module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "uiquickcompletionmodel_p.h"
#include <UiHelpers/UiProxyQmlModel>

class UiQuickCompletionModelPrivate
{
public:
    UiQuickCompletionModelPrivate(UiQuickCompletionModel *parent)
        : proxy(new UiProxyQmlModel(parent)) {}

    QVariant source;
    QString roleName;
    UiProxyQmlModel *proxy;
};


UiQuickCompletionModel::UiQuickCompletionModel(QObject *parent)
    : UiCompletionModel(parent)
    , d_ptr(new UiQuickCompletionModelPrivate(this))
{
}

UiQuickCompletionModel::~UiQuickCompletionModel()
{
}

QVariant UiQuickCompletionModel::source() const
{
    Q_D(const UiQuickCompletionModel);

    return d->source;
}

void UiQuickCompletionModel::setSource(const QVariant& source)
{
    Q_D(UiQuickCompletionModel);

    if (source == d->source)
        return;

    setSourceModel(0); // Disconnect all signals from old model
    UiProxyQmlModel::ListType type = d->proxy->updateSource(source);
    if (type == UiProxyQmlModel::ArrayList)
        setCompletionRole(Qt::DisplayRole);
    else if (type == UiProxyQmlModel::QuickList) {
        int role = d->proxy->roleNames().key(d->roleName.toAscii(), -1);
        if (role != -1)
            setCompletionRole(role);
    }
    setSourceModel(d->proxy);

    d->source = source;
    emit sourceModelChanged();
}


QString UiQuickCompletionModel::completionRoleName() const
{
    Q_D(const UiQuickCompletionModel);

    return d->roleName;
}

void UiQuickCompletionModel::setCompletionRoleName(const QString &roleName)
{
    Q_D(UiQuickCompletionModel);

    if (roleName == d->roleName)
        return;

    int role = d->proxy->roleNames().key(roleName.toAscii(), -1);
    if (role != -1)
        setCompletionRole(role);

    d->roleName = roleName;
    emit completionRoleNameChanged();
}
