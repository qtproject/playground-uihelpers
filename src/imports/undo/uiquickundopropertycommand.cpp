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

#include "uiquickundopropertycommand_p.h"

typedef QPair<QByteArray, QVariant> PropertyState;
typedef QList<PropertyState> TargetState;

class UndoPropertyCommand : public BaseUndoCommand
{
public:
    UndoPropertyCommand(QObject*, UiQuickUndoPropertyCommand*);
    ~UndoPropertyCommand();

    void undo();
    void redo();

    bool delayPush() const;

private:
    void saveState(TargetState& state);
    void applyState(TargetState& state);

    TargetState m_undoState;
    TargetState m_redoState;
    QObject *m_target;
    UiQuickUndoPropertyCommand *m_qmlObject;
};

UndoPropertyCommand::UndoPropertyCommand(QObject* t, UiQuickUndoPropertyCommand *q)
    : BaseUndoCommand()
    , m_undoState(TargetState())
    , m_redoState(TargetState())
    , m_target(t)
    , m_qmlObject(q)
{
    saveState(m_undoState);
}

UndoPropertyCommand::~UndoPropertyCommand()
{
}

void UndoPropertyCommand::saveState(TargetState& state)
{
    foreach (const QVariant& var, m_qmlObject->properties()) {
        QByteArray propertyName = var.toByteArray();
        state.append(qMakePair(propertyName, m_target->property(propertyName.data())));
    }
}

void UndoPropertyCommand::applyState(TargetState& state)
{
    foreach (PropertyState pair, state)
        m_target->setProperty(pair.first, pair.second);
}

void UndoPropertyCommand::undo()
{
    applyState(m_undoState);
}

void UndoPropertyCommand::redo()
{
    if (m_redoState.empty())
        saveState(m_redoState);
    else
        applyState(m_redoState);
}

bool UndoPropertyCommand::delayPush() const
{
    return true;
}

UiQuickUndoPropertyCommand::UiQuickUndoPropertyCommand(QObject *parent)
    : UiQuickBaseUndoCommand(parent)
    , m_properties(QVariantList())
{
}

UiQuickUndoPropertyCommand::~UiQuickUndoPropertyCommand()
{
}

QVariantList UiQuickUndoPropertyCommand::properties() const
{
    return m_properties;
}

void UiQuickUndoPropertyCommand::setProperties(const QVariantList& prop)
{
    m_properties = prop;
    emit propertiesChanged();
}

BaseUndoCommand *UiQuickUndoPropertyCommand::create(QObject *target)
{
    return new UndoPropertyCommand(target, this);
}

