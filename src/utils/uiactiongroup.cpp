/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "uiactiongroup.h"

#ifndef QT_NO_ACTION

#include "uiaction_p.h"
#include "qlist.h"

QT_BEGIN_NAMESPACE_UIHELPERS

class UiActionGroupPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UiActionGroup)
public:
    UiActionGroupPrivate() : exclusive(1), enabled(1), visible(1)  { }
    QList<UiAction *> actions;
    QPointer<UiAction> current;
    uint exclusive : 1;
    uint enabled : 1;
    uint visible : 1;

private:
    void _q_actionTriggered();  //private slot
    void _q_actionChanged();    //private slot
};

void UiActionGroupPrivate::_q_actionChanged()
{
    Q_Q(UiActionGroup);
    UiAction *action = qobject_cast<UiAction*>(q->sender());
    Q_ASSERT_X(action != 0, "QWidgetGroup::_q_actionChanged", "internal error");
    if (exclusive) {
        if (action->isChecked()) {
            if (action != current) {
                if (current)
                    current->setChecked(false);
                current = action;
            }
        } else if (action == current) {
            current = 0;
        }
    }
}

void UiActionGroupPrivate::_q_actionTriggered()
{
    Q_Q(UiActionGroup);
    UiAction *action = qobject_cast<UiAction*>(q->sender());
    Q_ASSERT_X(action != 0, "QWidgetGroup::_q_actionTriggered", "internal error");
    emit q->triggered(action);
}

/*!
    \class QActionGroup
    \brief The QActionGroup class groups actions together.

    \ingroup mainwindow-classes
    \inmodule QtWidgets

    In some situations it is useful to group QAction objects together.
    For example, if you have a \gui{Left Align} action, a \gui{Right
    Align} action, a \gui{Justify} action, and a \gui{Center} action,
    only one of these actions should be active at any one time. One
    simple way of achieving this is to group the actions together in
    an action group.

    Here's a example (from the \l{mainwindows/menus}{Menus} example):

    \snippet examples/mainwindows/menus/mainwindow.cpp 6

    Here we create a new action group. Since the action group is
    exclusive by default, only one of the actions in the group is
    checked at any one time.

    \img qactiongroup-align.png Alignment options in a QMenu

    A QActionGroup emits an triggered() signal when one of its
    actions is chosen. Each action in an action group emits its
    triggered() signal as usual.

    As stated above, an action group is \l exclusive by default; it
    ensures that only one checkable action is active at any one time.
    If you want to group checkable actions without making them
    exclusive, you can turn of exclusiveness by calling
    setExclusive(false).

    Actions can be added to an action group using addAction(), but it
    is usually more convenient to specify a group when creating
    actions; this ensures that actions are automatically created with
    a parent. Actions can be visually separated from each other by
    adding a separator action to the group; create an action and use
    QAction's \l {QAction::}{setSeparator()} function to make it
    considered a separator. Action groups are added to widgets with
    the QWidget::addActions() function.

    \sa QAction
*/

/*!
    Constructs an action group for the \a parent object.

    The action group is exclusive by default. Call setExclusive(false)
    to make the action group non-exclusive.
*/
UiActionGroup::UiActionGroup(QObject* parent) : QObject(*new UiActionGroupPrivate, parent)
{
}

/*!
    Destroys the action group.
*/
UiActionGroup::~UiActionGroup()
{
}

/*!
    \fn QAction *QActionGroup::addAction(QAction *action)

    Adds the \a action to this group, and returns it.

    Normally an action is added to a group by creating it with the
    group as its parent, so this function is not usually used.

    \sa QAction::setActionGroup()
*/
UiAction *UiActionGroup::addAction(UiAction* a)
{
    Q_D(UiActionGroup);
    if (!d->actions.contains(a)) {
        d->actions.append(a);
        QObject::connect(a, SIGNAL(triggered()), this, SLOT(_q_actionTriggered()));
        QObject::connect(a, SIGNAL(changed()), this, SLOT(_q_actionChanged()));
    }
    if (!a->d_func()->forceDisabled) {
        a->setEnabled(d->enabled);
        a->d_func()->forceDisabled = false;
    }
    if (!a->d_func()->forceInvisible) {
        a->setVisible(d->visible);
        a->d_func()->forceInvisible = false;
    }
    if (a->isChecked())
        d->current = a;
    UiActionGroup *oldGroup = a->d_func()->group;
    if (oldGroup != this) {
        if (oldGroup)
            oldGroup->removeAction(a);
        a->d_func()->group = this;
    }
    return a;
}

/*!
    Creates and returns an action with \a text.  The newly created
    action is a child of this action group.

    Normally an action is added to a group by creating it with the
    group as parent, so this function is not usually used.

    \sa QAction::setActionGroup()
*/
UiAction *UiActionGroup::addAction(const QString &text)
{
    return new UiAction(text, this);
}

/*!
  Removes the \a action from this group. The action will have no
  parent as a result.

  \sa QAction::setActionGroup()
*/
void UiActionGroup::removeAction(UiAction *action)
{
    Q_D(UiActionGroup);
    if (d->actions.removeAll(action)) {
        if (action == d->current)
            d->current = 0;
        QObject::disconnect(action, SIGNAL(triggered()), this, SLOT(_q_actionTriggered()));
        QObject::disconnect(action, SIGNAL(changed()), this, SLOT(_q_actionChanged()));
        action->d_func()->group = 0;
    }
}

/*!
    Returns the list of this groups's actions. This may be empty.
*/
QList<UiAction*> UiActionGroup::actions() const
{
    Q_D(const UiActionGroup);
    return d->actions;
}

/*!
    \property QActionGroup::exclusive
    \brief whether the action group does exclusive checking

    If exclusive is true, only one checkable action in the action group
    can ever be active at any time. If the user chooses another
    checkable action in the group, the one they chose becomes active and
    the one that was active becomes inactive.

    \sa QAction::checkable
*/
void UiActionGroup::setExclusive(bool b)
{
    Q_D(UiActionGroup);
    d->exclusive = b;
}

bool UiActionGroup::isExclusive() const
{
    Q_D(const UiActionGroup);
    return d->exclusive;
}

/*!
    \fn void QActionGroup::setDisabled(bool b)

    This is a convenience function for the \l enabled property, that
    is useful for signals--slots connections. If \a b is true the
    action group is disabled; otherwise it is enabled.
*/

/*!
    \property QActionGroup::enabled
    \brief whether the action group is enabled

    Each action in the group will be enabled or disabled unless it
    has been explicitly disabled.

    \sa QAction::setEnabled()
*/
void UiActionGroup::setEnabled(bool b)
{
    Q_D(UiActionGroup);
    d->enabled = b;
    for (QList<UiAction*>::const_iterator it = d->actions.constBegin(); it != d->actions.constEnd(); ++it) {
        if (!(*it)->d_func()->forceDisabled) {
            (*it)->setEnabled(b);
            (*it)->d_func()->forceDisabled = false;
        }
    }
}

bool UiActionGroup::isEnabled() const
{
    Q_D(const UiActionGroup);
    return d->enabled;
}

/*!
  Returns the currently checked action in the group, or 0 if none
  are checked.
*/
UiAction *UiActionGroup::checkedAction() const
{
    Q_D(const UiActionGroup);
    return d->current;
}

/*!
    \property QActionGroup::visible
    \brief whether the action group is visible

    Each action in the action group will match the visible state of
    this group unless it has been explicitly hidden.

    \sa QAction::setEnabled()
*/
void UiActionGroup::setVisible(bool b)
{
    Q_D(UiActionGroup);
    d->visible = b;
    for (QList<UiAction*>::Iterator it = d->actions.begin(); it != d->actions.end(); ++it) {
        if (!(*it)->d_func()->forceInvisible) {
            (*it)->setVisible(b);
            (*it)->d_func()->forceInvisible = false;
        }
    }
}

bool UiActionGroup::isVisible() const
{
    Q_D(const UiActionGroup);
    return d->visible;
}

/*!
    \fn void QActionGroup::triggered(QAction *action)

    This signal is emitted when the given \a action in the action
    group is activated by the user; for example, when the user clicks
    a menu option, toolbar button, or presses an action's shortcut key
    combination.

    Connect to this signal for command actions.

    \sa QAction::activate()
*/

/*!
    \fn void QActionGroup::add(QAction* a)

    Use addAction() instead.
*/

/*!
    \fn void QActionGroup::addSeparator()

    Normally you add a separator to the menus or widgets to which
    actions are added, so this function is very rarely needed.

    \oldcode
    actionGroup->addSeparator();
    \newcode
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    actionGroup->addAction(separator);
    \endcode
*/


QT_END_NAMESPACE_UIHELPERS

#include "moc_uiactiongroup.cpp"

#endif // QT_NO_ACTION
