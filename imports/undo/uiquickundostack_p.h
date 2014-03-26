/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT)
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

#ifndef UIQUICKUNDOSTACK_H
#define UIQUICKUNDOSTACK_H

#include <QtQml/QQmlComponent>
#include <UiHelpers/UiUndoStack>


QT_USE_NAMESPACE_UIHELPERS;

class UiQuickBaseUndoCommand;
class UiQuickUndoStackPrivate;
class UiQuickUndoStack : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int undoLimit READ undoLimit WRITE setUndoLimit NOTIFY undoLimitChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    UiQuickUndoStack(QObject *parent = 0);
    ~UiQuickUndoStack();

    int undoLimit() const;
    void setUndoLimit(int limit);

    bool canUndo() const;
    bool canRedo() const;
    int count() const;

    Q_INVOKABLE void clear();

public slots:
    void push(UiQuickBaseUndoCommand *cmd, QObject *target);
    void undo();
    void redo();

Q_SIGNALS:
    void undoLimitChanged();
    void canUndoChanged();
    void canRedoChanged();
    void countChanged();

private:
    Q_DISABLE_COPY(UiQuickUndoStack)
    Q_DECLARE_PRIVATE(UiQuickUndoStack)

    QScopedPointer<UiQuickUndoStackPrivate> d_ptr;
};

#endif
