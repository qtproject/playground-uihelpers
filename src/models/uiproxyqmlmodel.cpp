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

#ifndef QT_NO_PROXYQMLMODEL

#include "uiproxyqmlmodel.h"
#include <QtQml/private/qquicklistmodel_p.h>

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
    } else if (QQuickListModel *list = qvariant_cast<QQuickListModel*>(sourceModel)) {
        createFromQuickList(list);
        return QuickList;
    }

    return InvalidList;
}

void UiProxyQmlModel::createFromList(const QVariantList &list)
{
    QHash<int, QByteArray> roleNames;

    roleNames[Qt::DisplayRole] = "display";
    setRoleNames(roleNames);

    foreach (const QVariant& var, list) {
        QStandardItem *item = new QStandardItem();
        item->setData(var, Qt::DisplayRole);
        item->setFlags(Qt::ItemIsSelectable);
        appendRow(item);
    }
}

void UiProxyQmlModel::createFromQuickList(QQuickListModel *list)
{
    setRoleNames(list->roleNames());

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
