/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QACTIONGROUP_H
#define QACTIONGROUP_H

#include "uihelpersglobal.h"
#include <UiHelpers/uiaction.h>
#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_UIHELPERS


#ifndef QT_NO_ACTION

class UiActionGroupPrivate;

class UIHELPERS_EXPORT UiActionGroup : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UiActionGroup)

    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

public:
    explicit UiActionGroup(QObject* parent);
    ~UiActionGroup();

    UiAction *addAction(UiAction* a);
    UiAction *addAction(const QString &text);
    void removeAction(UiAction *a);
    QList<UiAction*> actions() const;

    UiAction *checkedAction() const;
    bool isExclusive() const;
    bool isEnabled() const;
    bool isVisible() const;


public Q_SLOTS:
    void setEnabled(bool);
    inline void setDisabled(bool b) { setEnabled(!b); }
    void setVisible(bool);
    void setExclusive(bool);

Q_SIGNALS:
    void triggered(UiAction *);

private:
    Q_DISABLE_COPY(UiActionGroup)
    Q_PRIVATE_SLOT(d_func(), void _q_actionTriggered())
    Q_PRIVATE_SLOT(d_func(), void _q_actionChanged())
};

#endif // QT_NO_ACTION

QT_END_NAMESPACE_UIHELPERS

QT_END_HEADER

#endif // QACTIONGROUP_H
