/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UIQUICKUNDOCOMMAND_H
#define UIQUICKUNDOCOMMAND_H

#include <UiHelpers/UiUndoStack>
#include <QVariant>

QT_USE_NAMESPACE_UIHELPERS;

class UiQuickBaseUndoCommand : public QObject
{
    Q_OBJECT

public:
    UiQuickBaseUndoCommand(QObject *parent = 0);
    ~UiQuickBaseUndoCommand();
};

// ------- //

class UiQuickUndoCommand : public UiQuickBaseUndoCommand
{
    Q_OBJECT

public:
    UiQuickUndoCommand(QObject *parent = 0);
    ~UiQuickUndoCommand();

signals:
    void undo(QObject *target);
    void redo(QObject *target);
    void commandDestroyed(QObject *target);
};

class UndoCommand : public UiUndoCommand
{
public:
    UndoCommand(QObject* target, UiQuickUndoCommand *m_qmlObject);
    ~UndoCommand();

    void undo();
    void redo();

private:
    QObject *m_target;
    UiQuickUndoCommand *m_qmlObject;
};

// -----------------//

class UiQuickUndoPropertyCommand : public UiQuickBaseUndoCommand
{
    Q_OBJECT

    Q_PROPERTY(QVariantList properties READ properties WRITE setProperties NOTIFY propertiesChanged)

public:
    UiQuickUndoPropertyCommand(QObject *parent = 0);
    ~UiQuickUndoPropertyCommand();

    QVariantList properties() const;
    void setProperties(const QVariantList& prop);

signals:
    void propertiesChanged();

private:
    QVariantList m_properties;
};

typedef QPair<QByteArray, QVariant> PropertyState;
typedef QList<PropertyState> TargetState;

class UndoPropertyCommand : public UiUndoCommand
{

public:
    UndoPropertyCommand(QObject*, UiQuickUndoPropertyCommand*);
    ~UndoPropertyCommand();

    void undo();
    void redo();

private:
    void saveState(TargetState& state);
    void applyState(TargetState& state);

    TargetState m_undoState;
    TargetState m_redoState;
    QObject *m_target;
    UiQuickUndoPropertyCommand *m_qmlObject;
};

#endif
