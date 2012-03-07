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

#include <QtCore/qdebug.h>
#include "uiundostack.h"
#include "uiundogroup.h"
#include "uiundostack_p.h"

#ifndef QT_NO_UNDOCOMMAND

QT_BEGIN_NAMESPACE_UIHELPERS

/*!
    \class UiUndoCommand
    \brief The UiUndoCommand class is the base class of all commands stored on a UiUndoStack.
    \since 4.2

    \inmodule UiHelpers

    For an overview of Qt's Undo Framework, see the
    \l{Overview of Qt's Undo Framework}{overview document}.

    A UiUndoCommand represents a single editing action on a document; for example,
    inserting or deleting a block of text in a text editor. UiUndoCommand can apply
    a change to the document with redo() and undo the change with undo(). The
    implementations for these functions must be provided in a derived class.

    \snippet doc/src/snippets/code/src_gui_util_UiUndostack.cpp 0

    A UiUndoCommand has an associated text(). This is a short string
    describing what the command does. It is used to update the text
    properties of the stack's undo and redo actions; see
    UiUndoStack::createUndoAction() and UiUndoStack::createRedoAction().

    UiUndoCommand objects are owned by the stack they were pushed on.
    UiUndoStack deletes a command if it has been undone and a new command is pushed. For example:

\snippet doc/src/snippets/code/src_gui_util_UiUndostack.cpp 1

    In effect, when a command is pushed, it becomes the top-most command
    on the stack.

    To support command compression, UiUndoCommand has an id() and the virtual function
    mergeWith(). These functions are used by UiUndoStack::push().

    To support command macros, a UiUndoCommand object can have any number of child
    commands. Undoing or redoing the parent command will cause the child
    commands to be undone or redone. A command can be assigned
    to a parent explicitly in the constructor. In this case, the command
    will be owned by the parent.

    The parent in this case is usually an empty command, in that it doesn't
    provide its own implementation of undo() and redo(). Instead, it uses
    the base implementations of these functions, which simply call undo() or
    redo() on all its children. The parent should, however, have a meaningful
    text().

    \snippet doc/src/snippets/code/src_gui_util_UiUndostack.cpp 2

    Another way to create macros is to use the convenience functions
    UiUndoStack::beginMacro() and UiUndoStack::endMacro().

    \sa UiUndoStack
*/

/*!
    Constructs a UiUndoCommand object with the given \a parent and \a text.

    If \a parent is not 0, this command is appended to parent's child list.
    The parent command then owns this command and will delete it in its
    destructor.

    \sa ~UiUndoCommand()
*/

UiUndoCommand::UiUndoCommand(const QString &text, UiUndoCommand *parent)
{
    d = new UiUndoCommandPrivate;
    if (parent != 0)
        parent->d->child_list.append(this);
    setText(text);
}

/*!
    Constructs a UiUndoCommand object with parent \a parent.

    If \a parent is not 0, this command is appended to parent's child list.
    The parent command then owns this command and will delete it in its
    destructor.

    \sa ~UiUndoCommand()
*/

UiUndoCommand::UiUndoCommand(UiUndoCommand *parent)
{
    d = new UiUndoCommandPrivate;
    if (parent != 0)
        parent->d->child_list.append(this);
}

/*!
    Destroys the UiUndoCommand object and all child commands.

    \sa UiUndoCommand()
*/

UiUndoCommand::~UiUndoCommand()
{
    qDeleteAll(d->child_list);
    delete d;
}

/*!
    Returns the ID of this command.

    A command ID is used in command compression. It must be an integer unique to
    this command's class, or -1 if the command doesn't support compression.

    If the command supports compression this function must be overridden in the
    derived class to return the correct ID. The base implementation returns -1.

    UiUndoStack::push() will only try to merge two commands if they have the
    same ID, and the ID is not -1.

    \sa mergeWith(), UiUndoStack::push()
*/

int UiUndoCommand::id() const
{
    return -1;
}

/*!
    Attempts to merge this command with \a command. Returns true on
    success; otherwise returns false.

    If this function returns true, calling this command's redo() must have the same
    effect as redoing both this command and \a command.
    Similarly, calling this command's undo() must have the same effect as undoing
    \a command and this command.

    UiUndoStack will only try to merge two commands if they have the same id, and
    the id is not -1.

    The default implementation returns false.

    \snippet doc/src/snippets/code/src_gui_util_UiUndostack.cpp 3

    \sa id() UiUndoStack::push()
*/

bool UiUndoCommand::mergeWith(const UiUndoCommand *command)
{
    Q_UNUSED(command);
    return false;
}

/*!
    Applies a change to the document. This function must be implemented in
    the derived class. Calling UiUndoStack::push(),
    UiUndoStack::undo() or UiUndoStack::redo() from this function leads to
    undefined beahavior.

    The default implementation calls redo() on all child commands.

    \sa undo()
*/

void UiUndoCommand::redo()
{
    for (int i = 0; i < d->child_list.size(); ++i)
        d->child_list.at(i)->redo();
}

/*!
    Reverts a change to the document. After undo() is called, the state of
    the document should be the same as before redo() was called. This function must
    be implemented in the derived class. Calling UiUndoStack::push(),
    UiUndoStack::undo() or UiUndoStack::redo() from this function leads to
    undefined beahavior.

    The default implementation calls undo() on all child commands in reverse order.

    \sa redo()
*/

void UiUndoCommand::undo()
{
    for (int i = d->child_list.size() - 1; i >= 0; --i)
        d->child_list.at(i)->undo();
}

/*!
    Returns a short text string describing what this command does; for example,
    "insert text".

    The text is used for names of items in UiUndoView.

    \sa actionText(), setText(), UiUndoStack::createUndoAction(), UiUndoStack::createRedoAction()
*/

QString UiUndoCommand::text() const
{
    return d->text;
}

/*!
    \since 4.8

    Returns a short text string describing what this command does; for example,
    "insert text".

    The text is used when the text properties of the stack's undo and redo
    actions are updated.

    \sa text(), setText(), UiUndoStack::createUndoAction(), UiUndoStack::createRedoAction()
*/

QString UiUndoCommand::actionText() const
{
    return d->actionText;
}

/*!
    Sets the command's text to be the \a text specified.

    The specified text should be a short user-readable string describing what this
    command does.

    If you need to have two different strings for text() and actionText(), separate
    them with "\n" and pass into this function. Even if you do not use this feature
    for English strings during development, you can still let translators use two
    different strings in order to match specific languages' needs.
    The described feature and the function actionText() are available since Qt 4.8.

    \sa text() actionText() UiUndoStack::createUndoAction() UiUndoStack::createRedoAction()
*/

void UiUndoCommand::setText(const QString &text)
{
    int cdpos = text.indexOf(QLatin1Char('\n'));
    if (cdpos > 0) {
        d->text = text.left(cdpos);
        d->actionText = text.mid(cdpos + 1);
    } else {
        d->text = text;
        d->actionText = text;
    }
}

/*!
    \since 4.4

    Returns the number of child commands in this command.

    \sa child()
*/

int UiUndoCommand::childCount() const
{
    return d->child_list.count();
}

/*!
    \since 4.4

    Returns the child command at \a index.

    \sa childCount(), UiUndoStack::command()
*/

const UiUndoCommand *UiUndoCommand::child(int index) const
{
    if (index < 0 || index >= d->child_list.count())
        return 0;
    return d->child_list.at(index);
}

#endif // QT_NO_UNDOCOMMAND

#ifndef QT_NO_UNDOSTACK

/*!
    \class UiUndoStack
    \brief The UiUndoStack class is a stack of UiUndoCommand objects.
    \since 4.2

    \inmodule UiHelpers

    For an overview of Qt's Undo Framework, see the
    \l{Overview of Qt's Undo Framework}{overview document}.

    An undo stack maintains a stack of commands that have been applied to a
    document.

    New commands are pushed on the stack using push(). Commands can be
    undone and redone using undo() and redo(), or by triggering the
    actions returned by createUndoAction() and createRedoAction().

    UiUndoStack keeps track of the \a current command. This is the command
    which will be executed by the next call to redo(). The index of this
    command is returned by index(). The state of the edited object can be
    rolled forward or back using setIndex(). If the top-most command on the
    stack has already been redone, index() is equal to count().

    UiUndoStack provides support for undo and redo actions, command
    compression, command macros, and supports the concept of a
    \e{clean state}.

    \section1 Undo and Redo Actions

    UiUndoStack provides convenient undo and redo QAction objects, which
    can be inserted into a menu or a toolbar. When commands are undone or
    redone, UiUndoStack updates the text properties of these actions
    to reflect what change they will trigger. The actions are also disabled
    when no command is available for undo or redo. These actions
    are returned by UiUndoStack::createUndoAction() and UiUndoStack::createRedoAction().

    \section1 Command Compression and Macros

    Command compression is useful when several commands can be compressed
    into a single command that can be undone and redone in a single operation.
    For example, when a user types a character in a text editor, a new command
    is created. This command inserts the character into the document at the
    cursor position. However, it is more convenient for the user to be able
    to undo or redo typing of whole words, sentences, or paragraphs.
    Command compression allows these single-character commands to be merged
    into a single command which inserts or deletes sections of text.
    For more information, see UiUndoCommand::mergeWith() and push().

    A command macro is a sequence of commands, all of which are undone and
    redone in one go. Command macros are created by giving a command a list
    of child commands.
    Undoing or redoing the parent command will cause the child commands to
    be undone or redone. Command macros may be created explicitly
    by specifying a parent in the UiUndoCommand constructor, or by using the
    convenience functions beginMacro() and endMacro().

    Although command compression and macros appear to have the same effect to the
    user, they often have different uses in an application. Commands that
    perform small changes to a document may be usefully compressed if there is
    no need to individually record them, and if only larger changes are relevant
    to the user.
    However, for commands that need to be recorded individually, or those that
    cannot be compressed, it is useful to use macros to provide a more convenient
    user experience while maintaining a record of each command.

    \section1 Clean State

    UiUndoStack supports the concept of a clean state. When the
    document is saved to disk, the stack can be marked as clean using
    setClean(). Whenever the stack returns to this state through undoing and
    redoing commands, it emits the signal cleanChanged(). This signal
    is also emitted when the stack leaves the clean state. This signal is
    usually used to enable and disable the save actions in the application,
    and to update the document's title to reflect that it contains unsaved
    changes.

    \sa UiUndoCommand, UiUndoView
*/

#ifndef QT_NO_ACTION

UiUndoAction::UiUndoAction(const QString &prefix, QObject *parent)
    : QAction(parent)
{
    m_prefix = prefix;
}

void UiUndoAction::setPrefixedText(const QString &text)
{
    if (m_defaultText.isEmpty()) {
        QString s = m_prefix;
        if (!m_prefix.isEmpty() && !text.isEmpty())
            s.append(QLatin1Char(' '));
        s.append(text);
        setText(s);
    } else {
        if (text.isEmpty())
            setText(m_defaultText);
        else
            setText(m_prefix.arg(text));
    }
}

void UiUndoAction::setTextFormat(const QString &textFormat, const QString &defaultText)
{
    m_prefix = textFormat;
    m_defaultText = defaultText;
}

#endif // QT_NO_ACTION

/*! \internal
    Sets the current index to \a idx, emitting appropriate signals. If \a clean is true,
    makes \a idx the clean index as well.
*/

void UiUndoStackPrivate::setIndex(int idx, bool clean)
{
    Q_Q(UiUndoStack);

    bool was_clean = index == clean_index;

    if (idx != index) {
        index = idx;
        emit q->indexChanged(index);
        emit q->canUndoChanged(q->canUndo());
        emit q->undoTextChanged(q->undoText());
        emit q->canRedoChanged(q->canRedo());
        emit q->redoTextChanged(q->redoText());
    }

    if (clean)
        clean_index = index;

    bool is_clean = index == clean_index;
    if (is_clean != was_clean)
        emit q->cleanChanged(is_clean);
}

/*! \internal
    If the number of commands on the stack exceedes the undo limit, deletes commands from
    the bottom of the stack.

    Returns true if commands were deleted.
*/

bool UiUndoStackPrivate::checkUndoLimit()
{
    if (undo_limit <= 0 || !macro_stack.isEmpty() || undo_limit >= command_list.count())
        return false;

    int del_count = command_list.count() - undo_limit;

    for (int i = 0; i < del_count; ++i)
        delete command_list.takeFirst();

    index -= del_count;
    if (clean_index != -1) {
        if (clean_index < del_count)
            clean_index = -1; // we've deleted the clean command
        else
            clean_index -= del_count;
    }

    return true;
}

/*!
    Constructs an empty undo stack with the parent \a parent. The
    stack will initially be in the clean state. If \a parent is a
    UiUndoGroup object, the stack is automatically added to the group.

    \sa push()
*/

UiUndoStack::UiUndoStack(QObject *parent)
    : QObject(*(new UiUndoStackPrivate), parent)
{
#ifndef QT_NO_UNDOGROUP
    if (UiUndoGroup *group = qobject_cast<UiUndoGroup*>(parent))
        group->addStack(this);
#endif
}

/*!
    Destroys the undo stack, deleting any commands that are on it. If the
    stack is in a UiUndoGroup, the stack is automatically removed from the group.

    \sa UiUndoStack()
*/

UiUndoStack::~UiUndoStack()
{
#ifndef QT_NO_UNDOGROUP
    Q_D(UiUndoStack);
    if (d->group != 0)
        d->group->removeStack(this);
#endif
    clear();
}

/*!
    Clears the command stack by deleting all commands on it, and returns the stack
    to the clean state.

    Commands are not undone or redone; the state of the edited object remains
    unchanged.

    This function is usually used when the contents of the document are
    abandoned.

    \sa UiUndoStack()
*/

void UiUndoStack::clear()
{
    Q_D(UiUndoStack);

    if (d->command_list.isEmpty())
        return;

    bool was_clean = isClean();

    d->macro_stack.clear();
    qDeleteAll(d->command_list);
    d->command_list.clear();

    d->index = 0;
    d->clean_index = 0;

    emit indexChanged(0);
    emit canUndoChanged(false);
    emit undoTextChanged(QString());
    emit canRedoChanged(false);
    emit redoTextChanged(QString());

    if (!was_clean)
        emit cleanChanged(true);
}

/*!
    Pushes \a cmd on the stack or merges it with the most recently executed command.
    In either case, executes \a cmd by calling its redo() function.

    If \a cmd's id is not -1, and if the id is the same as that of the
    most recently executed command, UiUndoStack will attempt to merge the two
    commands by calling UiUndoCommand::mergeWith() on the most recently executed
    command. If UiUndoCommand::mergeWith() returns true, \a cmd is deleted.

    In all other cases \a cmd is simply pushed on the stack.

    If commands were undone before \a cmd was pushed, the current command and
    all commands above it are deleted. Hence \a cmd always ends up being the
    top-most on the stack.

    Once a command is pushed, the stack takes ownership of it. There
    are no getters to return the command, since modifying it after it has
    been executed will almost always lead to corruption of the document's
    state.

    \sa UiUndoCommand::id() UiUndoCommand::mergeWith()
*/

void UiUndoStack::push(UiUndoCommand *cmd)
{
    Q_D(UiUndoStack);
    cmd->redo();

    bool macro = !d->macro_stack.isEmpty();

    UiUndoCommand *cur = 0;
    if (macro) {
        UiUndoCommand *macro_cmd = d->macro_stack.last();
        if (!macro_cmd->d->child_list.isEmpty())
            cur = macro_cmd->d->child_list.last();
    } else {
        if (d->index > 0)
            cur = d->command_list.at(d->index - 1);
        while (d->index < d->command_list.size())
            delete d->command_list.takeLast();
        if (d->clean_index > d->index)
            d->clean_index = -1; // we've deleted the clean state
    }

    bool try_merge = cur != 0
                        && cur->id() != -1
                        && cur->id() == cmd->id()
                        && (macro || d->index != d->clean_index);

    if (try_merge && cur->mergeWith(cmd)) {
        delete cmd;
        if (!macro) {
            emit indexChanged(d->index);
            emit canUndoChanged(canUndo());
            emit undoTextChanged(undoText());
            emit canRedoChanged(canRedo());
            emit redoTextChanged(redoText());
        }
    } else {
        if (macro) {
            d->macro_stack.last()->d->child_list.append(cmd);
        } else {
            d->command_list.append(cmd);
            d->checkUndoLimit();
            d->setIndex(d->index + 1, false);
        }
    }
    emit stackChanged();
}

/*!
    Marks the stack as clean and emits cleanChanged() if the stack was
    not already clean.

    Whenever the stack returns to this state through the use of undo/redo
    commands, it emits the signal cleanChanged(). This signal is also
    emitted when the stack leaves the clean state.

    \sa isClean(), cleanIndex()
*/

void UiUndoStack::setClean()
{
    Q_D(UiUndoStack);
    if (!d->macro_stack.isEmpty()) {
        qWarning("UiUndoStack::setClean(): cannot set clean in the middle of a macro");
        return;
    }

    d->setIndex(d->index, true);
}

/*!
    If the stack is in the clean state, returns true; otherwise returns false.

    \sa setClean() cleanIndex()
*/

bool UiUndoStack::isClean() const
{
    Q_D(const UiUndoStack);
    if (!d->macro_stack.isEmpty())
        return false;
    return d->clean_index == d->index;
}

/*!
    Returns the clean index. This is the index at which setClean() was called.

    A stack may not have a clean index. This happens if a document is saved,
    some commands are undone, then a new command is pushed. Since
    push() deletes all the undone commands before pushing the new command, the stack
    can't return to the clean state again. In this case, this function returns -1.

    \sa isClean() setClean()
*/

int UiUndoStack::cleanIndex() const
{
    Q_D(const UiUndoStack);
    return d->clean_index;
}

/*!
    Undoes the command below the current command by calling UiUndoCommand::undo().
    Decrements the current command index.

    If the stack is empty, or if the bottom command on the stack has already been
    undone, this function does nothing.

    \sa redo() index()
*/

void UiUndoStack::undo()
{
    Q_D(UiUndoStack);
    if (d->index == 0)
        return;

    if (!d->macro_stack.isEmpty()) {
        qWarning("UiUndoStack::undo(): cannot undo in the middle of a macro");
        return;
    }

    int idx = d->index - 1;
    d->command_list.at(idx)->undo();
    d->setIndex(idx, false);
    emit stackChanged();
}

/*!
    Redoes the current command by calling UiUndoCommand::redo(). Increments the current
    command index.

    If the stack is empty, or if the top command on the stack has already been
    redone, this function does nothing.

    \sa undo() index()
*/

void UiUndoStack::redo()
{
    Q_D(UiUndoStack);
    if (d->index == d->command_list.size())
        return;

    if (!d->macro_stack.isEmpty()) {
        qWarning("UiUndoStack::redo(): cannot redo in the middle of a macro");
        return;
    }

    d->command_list.at(d->index)->redo();
    d->setIndex(d->index + 1, false);
    emit stackChanged();
}

/*!
    Returns the number of commands on the stack. Macro commands are counted as
    one command.

    \sa index() setIndex() command()
*/

int UiUndoStack::count() const
{
    Q_D(const UiUndoStack);
    return d->command_list.size();
}

/*!
    Returns the index of the current command. This is the command that will be
    executed on the next call to redo(). It is not always the top-most command
    on the stack, since a number of commands may have been undone.

    \sa undo() redo() count()
*/

int UiUndoStack::index() const
{
    Q_D(const UiUndoStack);
    return d->index;
}

/*!
    Repeatedly calls undo() or redo() until the current command index reaches
    \a idx. This function can be used to roll the state of the document forwards
    of backwards. indexChanged() is emitted only once.

    \sa index() count() undo() redo()
*/

void UiUndoStack::setIndex(int idx)
{
    Q_D(UiUndoStack);
    if (!d->macro_stack.isEmpty()) {
        qWarning("UiUndoStack::setIndex(): cannot set index in the middle of a macro");
        return;
    }

    if (idx < 0)
        idx = 0;
    else if (idx > d->command_list.size())
        idx = d->command_list.size();

    int i = d->index;
    while (i < idx)
        d->command_list.at(i++)->redo();
    while (i > idx)
        d->command_list.at(--i)->undo();

    d->setIndex(idx, false);
}

/*!
    Returns true if there is a command available for undo; otherwise returns false.

    This function returns false if the stack is empty, or if the bottom command
    on the stack has already been undone.

    Synonymous with index() == 0.

    \sa index() canRedo()
*/

bool UiUndoStack::canUndo() const
{
    Q_D(const UiUndoStack);
    if (!d->macro_stack.isEmpty())
        return false;
    return d->index > 0;
}

/*!
    Returns true if there is a command available for redo; otherwise returns false.

    This function returns false if the stack is empty or if the top command
    on the stack has already been redone.

    Synonymous with index() == count().

    \sa index() canUndo()
*/

bool UiUndoStack::canRedo() const
{
    Q_D(const UiUndoStack);
    if (!d->macro_stack.isEmpty())
        return false;
    return d->index < d->command_list.size();
}

/*!
    Returns the text of the command which will be undone in the next call to undo().

    \sa UiUndoCommand::actionText() redoText()
*/

QString UiUndoStack::undoText() const
{
    Q_D(const UiUndoStack);
    if (!d->macro_stack.isEmpty())
        return QString();
    if (d->index > 0)
        return d->command_list.at(d->index - 1)->actionText();
    return QString();
}

/*!
    Returns the text of the command which will be redone in the next call to redo().

    \sa UiUndoCommand::actionText() undoText()
*/

QString UiUndoStack::redoText() const
{
    Q_D(const UiUndoStack);
    if (!d->macro_stack.isEmpty())
        return QString();
    if (d->index < d->command_list.size())
        return d->command_list.at(d->index)->actionText();
    return QString();
}

#ifndef QT_NO_ACTION

/*!
    Creates an undo QAction object with the given \a parent.

    Triggering this action will cause a call to undo(). The text of this action
    is the text of the command which will be undone in the next call to undo(),
    prefixed by the specified \a prefix. If there is no command available for undo,
    this action will be disabled.

    If \a prefix is empty, the default template "Undo %1" is used instead of prefix.
    Before Qt 4.8, the prefix "Undo" was used by default.

    \sa createRedoAction(), canUndo(), UiUndoCommand::text()
*/

QAction *UiUndoStack::createUndoAction(QObject *parent, const QString &prefix) const
{
    UiUndoAction *result = new UiUndoAction(prefix, parent);
    if (prefix.isEmpty())
        result->setTextFormat(tr("Undo %1"), tr("Undo", "Default text for undo action"));

    result->setEnabled(canUndo());
    result->setPrefixedText(undoText());
    connect(this, SIGNAL(canUndoChanged(bool)),
            result, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(undoTextChanged(QString)),
            result, SLOT(setPrefixedText(QString)));
    connect(result, SIGNAL(triggered()), this, SLOT(undo()));
    return result;
}

/*!
    Creates an redo QAction object with the given \a parent.

    Triggering this action will cause a call to redo(). The text of this action
    is the text of the command which will be redone in the next call to redo(),
    prefixed by the specified \a prefix. If there is no command available for redo,
    this action will be disabled.

    If \a prefix is empty, the default template "Redo %1" is used instead of prefix.
    Before Qt 4.8, the prefix "Redo" was used by default.

    \sa createUndoAction(), canRedo(), UiUndoCommand::text()
*/

QAction *UiUndoStack::createRedoAction(QObject *parent, const QString &prefix) const
{
    UiUndoAction *result = new UiUndoAction(prefix, parent);
    if (prefix.isEmpty())
        result->setTextFormat(tr("Redo %1"), tr("Redo", "Default text for redo action"));

    result->setEnabled(canRedo());
    result->setPrefixedText(redoText());
    connect(this, SIGNAL(canRedoChanged(bool)),
            result, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoTextChanged(QString)),
            result, SLOT(setPrefixedText(QString)));
    connect(result, SIGNAL(triggered()), this, SLOT(redo()));
    return result;
}

#endif // QT_NO_ACTION

/*!
    Begins composition of a macro command with the given \a text description.

    An empty command described by the specified \a text is pushed on the stack.
    Any subsequent commands pushed on the stack will be appended to the empty
    command's children until endMacro() is called.

    Calls to beginMacro() and endMacro() may be nested, but every call to
    beginMacro() must have a matching call to endMacro().

    While a macro is composed, the stack is disabled. This means that:
    \list
    \i indexChanged() and cleanChanged() are not emitted,
    \i canUndo() and canRedo() return false,
    \i calling undo() or redo() has no effect,
    \i the undo/redo actions are disabled.
    \endlist

    The stack becomes enabled and appropriate signals are emitted when endMacro()
    is called for the outermost macro.

    \snippet doc/src/snippets/code/src_gui_util_UiUndostack.cpp 4

    This code is equivalent to:

    \snippet doc/src/snippets/code/src_gui_util_UiUndostack.cpp 5

    \sa endMacro()
*/

void UiUndoStack::beginMacro(const QString &text)
{
    Q_D(UiUndoStack);
    UiUndoCommand *cmd = new UiUndoCommand();
    cmd->setText(text);

    if (d->macro_stack.isEmpty()) {
        while (d->index < d->command_list.size())
            delete d->command_list.takeLast();
        if (d->clean_index > d->index)
            d->clean_index = -1; // we've deleted the clean state
        d->command_list.append(cmd);
    } else {
        d->macro_stack.last()->d->child_list.append(cmd);
    }
    d->macro_stack.append(cmd);

    if (d->macro_stack.count() == 1) {
        emit canUndoChanged(false);
        emit undoTextChanged(QString());
        emit canRedoChanged(false);
        emit redoTextChanged(QString());
    }
}

/*!
    Ends composition of a macro command.

    If this is the outermost macro in a set nested macros, this function emits
    indexChanged() once for the entire macro command.

    \sa beginMacro()
*/

void UiUndoStack::endMacro()
{
    Q_D(UiUndoStack);
    if (d->macro_stack.isEmpty()) {
        qWarning("UiUndoStack::endMacro(): no matching beginMacro()");
        return;
    }

    d->macro_stack.removeLast();

    if (d->macro_stack.isEmpty()) {
        d->checkUndoLimit();
        d->setIndex(d->index + 1, false);
    }
}

/*!
  \since 4.4

  Returns a const pointer to the command at \a index.

  This function returns a const pointer, because modifying a command,
  once it has been pushed onto the stack and executed, almost always
  causes corruption of the state of the document, if the command is
  later undone or redone.

  \sa UiUndoCommand::child()
*/
const UiUndoCommand *UiUndoStack::command(int index) const
{
    Q_D(const UiUndoStack);

    if (index < 0 || index >= d->command_list.count())
        return 0;
    return d->command_list.at(index);
}

/*!
    Returns the text of the command at index \a idx.

    \sa beginMacro()
*/

QString UiUndoStack::text(int idx) const
{
    Q_D(const UiUndoStack);

    if (idx < 0 || idx >= d->command_list.size())
        return QString();
    return d->command_list.at(idx)->text();
}

/*!
    \property UiUndoStack::undoLimit
    \brief the maximum number of commands on this stack.
    \since 4.3

    When the number of commands on a stack exceedes the stack's undoLimit, commands are
    deleted from the bottom of the stack. Macro commands (commands with child commands)
    are treated as one command. The default value is 0, which means that there is no
    limit.

    This property may only be set when the undo stack is empty, since setting it on a
    non-empty stack might delete the command at the current index. Calling setUndoLimit()
    on a non-empty stack prints a warning and does nothing.
*/

void UiUndoStack::setUndoLimit(int limit)
{
    Q_D(UiUndoStack);

    if (!d->command_list.isEmpty()) {
        qWarning("UiUndoStack::setUndoLimit(): an undo limit can only be set when the stack is empty");
        return;
    }

    if (limit == d->undo_limit)
        return;
    d->undo_limit = limit;
    d->checkUndoLimit();
}

int UiUndoStack::undoLimit() const
{
    Q_D(const UiUndoStack);

    return d->undo_limit;
}

/*!
    \property UiUndoStack::active
    \brief the active status of this stack.

    An application often has multiple undo stacks, one for each opened document. The active
    stack is the one associated with the currently active document. If the stack belongs
    to a UiUndoGroup, calls to UiUndoGroup::undo() or UiUndoGroup::redo() will be forwarded
    to this stack when it is active. If the UiUndoGroup is watched by a UiUndoView, the view
    will display the contents of this stack when it is active. If the stack does not belong to
    a UiUndoGroup, making it active has no effect.

    It is the programmer's responsibility to specify which stack is active by
    calling setActive(), usually when the associated document window receives focus.

    \sa UiUndoGroup
*/

void UiUndoStack::setActive(bool active)
{
#ifdef QT_NO_UNDOGROUP
    Q_UNUSED(active);
#else
    Q_D(UiUndoStack);

    if (d->group != 0) {
        if (active)
            d->group->setActiveStack(this);
        else if (d->group->activeStack() == this)
            d->group->setActiveStack(0);
    }
#endif
}

bool UiUndoStack::isActive() const
{
#ifdef QT_NO_UNDOGROUP
    return true;
#else
    Q_D(const UiUndoStack);
    return d->group == 0 || d->group->activeStack() == this;
#endif
}

/*!
    \fn void UiUndoStack::indexChanged(int idx)

    This signal is emitted whenever a command modifies the state of the document.
    This happens when a command is undone or redone. When a macro
    command is undone or redone, or setIndex() is called, this signal
    is emitted only once.

    \a idx specifies the index of the current command, ie. the command which will be
    executed on the next call to redo().

    \sa index() setIndex()
*/

/*!
    \fn void UiUndoStack::cleanChanged(bool clean)

    This signal is emitted whenever the stack enters or leaves the clean state.
    If \a clean is true, the stack is in a clean state; otherwise this signal
    indicates that it has left the clean state.

    \sa isClean() setClean()
*/

/*!
    \fn void UiUndoStack::undoTextChanged(const QString &undoText)

    This signal is emitted whenever the value of undoText() changes. It is
    used to update the text property of the undo action returned by createUndoAction().
    \a undoText specifies the new text.
*/

/*!
    \fn void UiUndoStack::canUndoChanged(bool canUndo)

    This signal is emitted whenever the value of canUndo() changes. It is
    used to enable or disable the undo action returned by createUndoAction().
    \a canUndo specifies the new value.
*/

/*!
    \fn void UiUndoStack::redoTextChanged(const QString &redoText)

    This signal is emitted whenever the value of redoText() changes. It is
    used to update the text property of the redo action returned by createRedoAction().
    \a redoText specifies the new text.
*/

/*!
    \fn void UiUndoStack::canRedoChanged(bool canRedo)

    This signal is emitted whenever the value of canRedo() changes. It is
    used to enable or disable the redo action returned by createRedoAction().
    \a canRedo specifies the new value.
*/

QStringList UiUndoStack::stack() const {
    Q_D(const UiUndoStack);
    QStringList list;
    foreach (UiUndoCommand *c, d->command_list) {
        list += c->text();
    }
    return list;
}


QT_END_NAMESPACE_UIHELPERS

#endif // QT_NO_UNDOSTACK
