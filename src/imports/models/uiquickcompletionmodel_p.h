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
