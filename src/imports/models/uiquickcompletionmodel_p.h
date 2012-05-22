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

#ifndef UIQUICKCOMPLETIONMODEL_H
#define UIQUICKCOMPLETIONMODEL_H

#include <UiHelpers/UiCompletionModel>

QT_USE_NAMESPACE_UIHELPERS;

class UiQuickCompletionModelPrivate;
class UiQuickCompletionModel : public UiCompletionModel
{
    Q_OBJECT

    Q_PROPERTY(QVariant sourceModel READ source WRITE setSource NOTIFY sourceModelChanged)
    Q_PROPERTY(QString completionRoleName READ completionRoleName WRITE setCompletionRoleName NOTIFY completionRoleNameChanged)

public:
    UiQuickCompletionModel(QObject *parent = 0);
    ~UiQuickCompletionModel();

    QVariant source() const;
    void setSource(const QVariant &source);
    QString completionRoleName() const;
    void setCompletionRoleName(const QString &roleName);

Q_SIGNALS:
    void sourceModelChanged();
    void completionRoleNameChanged();

private:
    Q_DISABLE_COPY(UiQuickCompletionModel)
    Q_DECLARE_PRIVATE(UiQuickCompletionModel)

    QScopedPointer<UiQuickCompletionModelPrivate> d_ptr;
};

#endif
