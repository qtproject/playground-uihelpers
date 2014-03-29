/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT).
** Contact: http://www.qt-project.org/
**
** This file is part of the UiHelpers playground module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_PROXYQMLMODEL

#include "uiproxyqmlmodel.h"
#include <QtQml/qqmlprivate.h>
#include <QtQml/private/qqmllistmodel_p.h>

QT_BEGIN_NAMESPACE_UIHELPERS

UiProxyQmlModel::UiProxyQmlModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

UiProxyQmlModel::ListType UiProxyQmlModel::updateSource(const QVariant &sourceModel)
{
    clear();

    if (sourceModel.type() == QVariant::List) {
        createFromList(sourceModel.toList());
        return ArrayList;
    } else if (QQmlListModel *list = qvariant_cast<QQmlListModel*>(sourceModel)) {
        createFromQuickList(list);
        return QuickList;
    }

    return InvalidList;
}


QHash<int, QByteArray> UiProxyQmlModel::roleNames()
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    return roles;
}

void UiProxyQmlModel::createFromList(const QVariantList &list)
{
    foreach (const QVariant& var, list) {
        QStandardItem *item = new QStandardItem();
        item->setData(var, Qt::DisplayRole);
        item->setFlags(Qt::ItemIsSelectable);
        appendRow(item);
    }
}

void UiProxyQmlModel::createFromQuickList(QQmlListModel *list)
{
    for (int i = 0; i < list->count(); i++) {
        QStandardItem *item = new QStandardItem();

        foreach (int role, list->roleNames().keys())
            item->setData(list->data(i, role), Qt::UserRole + role);
        item->setFlags(Qt::ItemIsSelectable);
        appendRow(item);
    }
}

QT_END_NAMESPACE_UIHELPERS

#endif // QT_NO_PROXYQMLMODEL
