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

#ifndef QACTION_P_H
#define QACTION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "UiHelpers/uiaction.h"
#include "private/qobject_p.h"
#include "uihelpersglobal.h"

QT_BEGIN_NAMESPACE_UIHELPERS

#ifndef QT_NO_ACTION


class QShortcutMap;

class UIHELPERS_EXPORT UiActionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UiAction)
public:
    UiActionPrivate();
    ~UiActionPrivate();

    static UiActionPrivate *get(UiAction *q)
    {
        return q->d_func();
    }

    QPointer<UiActionGroup> group;
    QString text;
    QString iconText;
    QString tooltip;
    QString statustip;
    QString whatsthis;
#ifndef QT_NO_SHORTCUT
    QKeySequence shortcut;
    QList<QKeySequence> alternateShortcuts;
#endif
    QVariant userData;
#ifndef QT_NO_SHORTCUT
    int shortcutId;
    QList<int> alternateShortcutIds;
    Qt::ShortcutContext shortcutContext;
    uint autorepeat : 1;
#endif
    uint enabled : 1, forceDisabled : 1;
    uint visible : 1, forceInvisible : 1;
    uint checkable : 1;
    uint checked : 1;

#ifndef QT_NO_SHORTCUT
    void redoGrab(QShortcutMap &map);
    void redoGrabAlternate(QShortcutMap &map);
    void setShortcutEnabled(bool enable, QShortcutMap &map);

    static QShortcutMap *globalMap;
#endif // QT_NO_SHORTCUT

    void sendDataChanged();
};

#endif // QT_NO_ACTION

QT_END_NAMESPACE_UIHELPERS

#endif // QACTION_P_H
