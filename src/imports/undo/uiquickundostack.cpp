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
#include "uiquickundocommand_p.h"
#include "uiquickundopropertycommand_p.h"

#include <QtQml/QQmlInfo>

class UiQuickUndoStackPrivate : public UiUndoStack
{
    Q_DECLARE_PUBLIC(UiQuickUndoStack)

public:
    UiQuickUndoStackPrivate(UiQuickUndoStack *q);
    ~UiQuickUndoStackPrivate();

    void setCurrentCommand(UiUndoCommand *cmd);
    void commit();

    UiQuickUndoStack *q_ptr;
    UiUndoCommand *currentCommand;
};

UiQuickUndoStackPrivate::UiQuickUndoStackPrivate(UiQuickUndoStack *q)
    : UiUndoStack(q)
    , q_ptr(q)
    , currentCommand(0)
{
}

UiQuickUndoStackPrivate::~UiQuickUndoStackPrivate()
{
}

void UiQuickUndoStackPrivate::setCurrentCommand(UiUndoCommand *cmd)
{
    Q_Q(UiQuickUndoStack);
    currentCommand = cmd;

    if (!canUndo())
        emit q->canUndoChanged();
}

void UiQuickUndoStackPrivate::commit()
{
    if (!currentCommand)
        return;

    push(currentCommand);
    setCurrentCommand(0);
}


UiQuickUndoStack::UiQuickUndoStack(QObject *parent)
    : QObject(parent)
    , d_ptr(new UiQuickUndoStackPrivate(this))
{
    Q_D(const UiQuickUndoStack);
    connect(d, SIGNAL(canUndoChanged(bool)), SIGNAL(canUndoChanged()));
    connect(d, SIGNAL(canRedoChanged(bool)), SIGNAL(canRedoChanged()));
}

UiQuickUndoStack::~UiQuickUndoStack()
{
}

int UiQuickUndoStack::undoLimit() const
{
    Q_D(const UiQuickUndoStack);
    return d->undoLimit();
}

void UiQuickUndoStack::setUndoLimit(int limit)
{
    Q_D(UiQuickUndoStack);

    int temp = d->undoLimit();
    d->setUndoLimit(limit);

    if (temp != limit)
        emit undoLimitChanged();
}

bool UiQuickUndoStack::canUndo() const
{
    Q_D(const UiQuickUndoStack);
    return d->canUndo() || (d->currentCommand != 0);
}

bool UiQuickUndoStack::canRedo() const
{
    Q_D(const UiQuickUndoStack);
    return d->canRedo();
}

int UiQuickUndoStack::count() const
{
    Q_D(const UiQuickUndoStack);
    if ((d->undoLimit() && (d->count() == d->undoLimit())) || !d->currentCommand)
        return d->count();
    else
        return d->count() + 1;
}

void UiQuickUndoStack::clear()
{
    Q_D(UiQuickUndoStack);
    d->clear();
    d->setCurrentCommand(0);
    emit countChanged();
}

void UiQuickUndoStack::push(UiQuickBaseUndoCommand *quickCommand, QObject *target)
{
    Q_D(UiQuickUndoStack);

    if (!quickCommand || !target)
        return; // XXX: notify error

    d->commit();

    int countTemp = count();

    BaseUndoCommand *undoCommand = quickCommand->create(target);
    if (undoCommand->delayPush())
        d->setCurrentCommand(undoCommand);
    else
        d->push(undoCommand);

    if (countTemp != count())
        emit countChanged();
}

void UiQuickUndoStack::undo()
{
    Q_D(UiQuickUndoStack);
    d->commit();
    d->undo();
}

void UiQuickUndoStack::redo()
{
    Q_D(UiQuickUndoStack);
    d->commit();
    d->redo();
}
