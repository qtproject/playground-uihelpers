/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UIHELPERS_UIUNDOSTACK_H
#define UIHELPERS_UIUNDOSTACK_H

#include "uihelpersglobal.h"
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QStringList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_UIHELPERS


class UiAction;
class UiUndoCommandPrivate;
class UiUndoStackPrivate;

#ifndef QT_NO_UNDOCOMMAND

class UIHELPERS_EXPORT UiUndoCommand
{
    UiUndoCommandPrivate *d;
public:
    explicit UiUndoCommand(UiUndoCommand *parent = 0);
    explicit UiUndoCommand(const QString &text, UiUndoCommand *parent = 0);
    virtual ~UiUndoCommand();

    virtual void undo();
    virtual void redo();

    Q_INVOKABLE QString text() const;
    QString actionText() const;
    void setText(const QString &text);

    virtual int id() const;
    virtual bool mergeWith(const UiUndoCommand *other);

    int childCount() const;
    const UiUndoCommand *child(int index) const;

private:
    Q_DISABLE_COPY(UiUndoCommand)
    friend class UiUndoStack;
};

#endif // QT_NO_UNDOCOMMAND

#ifndef QT_NO_UNDOSTACK

class UIHELPERS_EXPORT UiUndoStack : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UiUndoStack)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int undoLimit READ undoLimit WRITE setUndoLimit)
    Q_PROPERTY(QStringList stack READ stack NOTIFY stackChanged)

public:
    explicit UiUndoStack(QObject *parent = 0);
    ~UiUndoStack();
    void clear();

    void push(UiUndoCommand *cmd);

    QStringList stack() const;

    bool canUndo() const;
    bool canRedo() const;
    QString undoText() const;
    QString redoText() const;

    int count() const;
    int index() const;
    QString text(int idx) const;

#ifndef QT_NO_ACTION
    UiAction *createUndoAction(QObject *parent,
                                const QString &prefix = QString()) const;
    UiAction *createRedoAction(QObject *parent,
                                const QString &prefix = QString()) const;
#endif // QT_NO_ACTION

    bool isActive() const;
    bool isClean() const;
    int cleanIndex() const;

    void beginMacro(const QString &text);
    void endMacro();

    void setUndoLimit(int limit);
    int undoLimit() const;

    const UiUndoCommand *command(int index) const;

public Q_SLOTS:
    void setClean();
    void setIndex(int idx);
    void undo();
    void redo();
    void setActive(bool active = true);

Q_SIGNALS:
    void indexChanged(int idx);
    void cleanChanged(bool clean);
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);
    void undoTextChanged(const QString &undoText);
    void redoTextChanged(const QString &redoText);
    void stackChanged();

private:
    Q_DISABLE_COPY(UiUndoStack)
    friend class UiUndoGroup;
};

#endif // QT_NO_UNDOSTACK

QT_END_NAMESPACE_UIHELPERS

QT_END_HEADER

#endif // UIHELPERS_UIUNDOSTACK_H
