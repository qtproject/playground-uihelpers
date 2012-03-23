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

#include "uiquickundostack_p.h"
#include "uiquickundocommands_p.h"

UiQuickUndoStack::UiQuickUndoStack(QObject *parent)
    : QObject(parent)
    , m_stack(new UndoStack(this))
{
}

UiQuickUndoStack::~UiQuickUndoStack()
{
}

UndoStack::UndoStack(QObject *parent)
    : UiUndoStack(parent)
    , currentCommand(0)
{
}

UndoStack::~UndoStack()
{
}

void UiQuickUndoStack::push(UiQuickBaseUndoCommand *cmd, QObject *target)
{
    if (!cmd || !target)
        return; // XXX: notify error

    m_stack->commit();

    UiQuickUndoPropertyCommand *upc = qobject_cast<UiQuickUndoPropertyCommand *>(cmd);
    if (upc) {
        m_stack->currentCommand = new UndoPropertyCommand(target, upc);
    } else {
        UiQuickUndoCommand *uc = qobject_cast<UiQuickUndoCommand *>(cmd);
        m_stack->push(new UndoCommand(target, uc));
    }
}

void UiQuickUndoStack::undo()
{
    m_stack->commit();
    m_stack->undo();
}

void UiQuickUndoStack::redo()
{
    m_stack->commit();
    m_stack->redo();
}

void UndoStack::commit()
{
    if (!currentCommand)
        return;

    push(currentCommand);
    currentCommand = 0;
}
