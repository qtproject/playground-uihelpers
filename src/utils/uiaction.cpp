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

#include "uiaction.h"
#include "uiactiongroup.h"

#ifndef QT_NO_ACTION
#include "uiaction_p.h"
#include "qguiapplication.h"
#include "qlist.h"
#include "qdebug.h"
// #include <private/qshortcutmap_p.h>
#include <private/qguiapplication_p.h>

#define QAPP_CHECK(functionName) \
    if (!qApp) { \
        qWarning("QAction: Initialize QApplication before calling '" functionName "'."); \
        return; \
    }

QT_BEGIN_NAMESPACE_UIHELPERS

UiActionEvent::UiActionEvent(int type, UiAction *action, UiAction *before)
    : QEvent(static_cast<QEvent::Type>(type)), act(action), bef(before)
{}

/*! \internal
*/
UiActionEvent::~UiActionEvent()
{
}

/*
  internal: guesses a descriptive text from a text suited for a menu entry
 */
static QString qt_strippedText(QString s)
{
    s.remove( QString::fromLatin1("...") );
    int i = 0;
    while (i < s.size()) {
        ++i;
        if (s.at(i-1) != QLatin1Char('&'))
            continue;
        if (i < s.size() && s.at(i) == QLatin1Char('&'))
            ++i;
        s.remove(i-1,1);
    }
    return s.trimmed();
}

UiActionPrivate::UiActionPrivate() : group(0), enabled(1), forceDisabled(0),
                                   visible(1), forceInvisible(0), checkable(0), checked(0)
{
#ifndef QT_NO_SHORTCUT
    shortcutId = 0;
    shortcutContext = Qt::WindowShortcut;
    autorepeat = true;
#endif
}

UiActionPrivate::~UiActionPrivate()
{
}

void UiActionPrivate::sendDataChanged()
{
    Q_Q(UiAction);
    UiActionEvent e(QEvent::ActionChanged, q);
    QGuiApplication::sendEvent(q, &e);

    emit q->changed();
}

#ifndef QT_NO_SHORTCUT
void UiActionPrivate::redoGrab(QShortcutMap &map)
{
    Q_Q(UiAction);
    if (shortcutId)
        map.removeShortcut(shortcutId, q);
    if (shortcut.isEmpty())
        return;
    shortcutId = map.addShortcut(q, shortcut, shortcutContext, qWidgetShortcutContextMatcher);
    if (!enabled)
        map.setShortcutEnabled(false, shortcutId, q);
    if (!autorepeat)
        map.setShortcutAutoRepeat(false, shortcutId, q);
}

void UiActionPrivate::redoGrabAlternate(QShortcutMap &map)
{
    Q_Q(UiAction);
    for (int i = 0; i < alternateShortcutIds.count(); ++i) {
        if (const int id = alternateShortcutIds.at(i))
            map.removeShortcut(id, q);
    }
    alternateShortcutIds.clear();
    if (alternateShortcuts.isEmpty())
        return;
    for (int i = 0; i < alternateShortcuts.count(); ++i) {
        const QKeySequence& alternate = alternateShortcuts.at(i);
        if (!alternate.isEmpty())
            alternateShortcutIds.append(map.addShortcut(q, alternate, shortcutContext, qWidgetShortcutContextMatcher));
        else
            alternateShortcutIds.append(0);
    }
    if (!enabled) {
        for (int i = 0; i < alternateShortcutIds.count(); ++i) {
            const int id = alternateShortcutIds.at(i);
            map.setShortcutEnabled(false, id, q);
        }
    }
    if (!autorepeat) {
        for (int i = 0; i < alternateShortcutIds.count(); ++i) {
            const int id = alternateShortcutIds.at(i);
            map.setShortcutAutoRepeat(false, id, q);
        }
    }
}

void UiActionPrivate::setShortcutEnabled(bool enable, QShortcutMap &map)
{
    Q_Q(UiAction);
    if (shortcutId)
        map.setShortcutEnabled(enable, shortcutId, q);
    for (int i = 0; i < alternateShortcutIds.count(); ++i) {
        if (const int id = alternateShortcutIds.at(i))
            map.setShortcutEnabled(enable, id, q);
    }
}
#endif // QT_NO_SHORTCUT

/*!
    \class QAction
    \brief The QAction class provides an abstract user interface
    action that can be inserted into widgets.

    \ingroup mainwindow-classes
    \inmodule QtWidgets

    \omit
        * parent and widget are different
        * parent does not define context
    \endomit

    In applications many common commands can be invoked via menus,
    toolbar buttons, and keyboard shortcuts. Since the user expects
    each command to be performed in the same way, regardless of the
    user interface used, it is useful to represent each command as
    an \e action.

    Actions can be added to menus and toolbars, and will
    automatically keep them in sync. For example, in a word processor,
    if the user presses a Bold toolbar button, the Bold menu item
    will automatically be checked.

    Actions can be created as independent objects, but they may
    also be created during the construction of menus; the QMenu class
    contains convenience functions for creating actions suitable for
    use as menu items.

    A QAction may contain an icon, menu text, a shortcut, status text,
    "What's This?" text, and a tooltip. Most of these can be set in
    the constructor. They can also be set independently with
    setIcon(), setText(), setIconText(), setShortcut(),
    setStatusTip(), setWhatsThis(), and setToolTip(). For menu items,
    it is possible to set an individual font with setFont().

    Actions are added to widgets using QWidget::addAction() or
    QGraphicsWidget::addAction(). Note that an action must be added to a
    widget before it can be used; this is also true when the shortcut should
    be global (i.e., Qt::ApplicationShortcut as Qt::ShortcutContext).

    Once a QAction has been created it should be added to the relevant
    menu and toolbar, then connected to the slot which will perform
    the action. For example:

    \snippet examples/mainwindows/application/mainwindow.cpp 19
    \codeline
    \snippet examples/mainwindows/application/mainwindow.cpp 28
    \snippet examples/mainwindows/application/mainwindow.cpp 31

    We recommend that actions are created as children of the window
    they are used in. In most cases actions will be children of
    the application's main window.

    \sa QMenu, QToolBar, {Application Example}
*/

/*!
    \fn void QAction::trigger()

    This is a convenience slot that calls activate(Trigger).
*/

/*!
    Constructs an action with \a parent. If \a parent is an action
    group the action will be automatically inserted into the group.
*/
UiAction::UiAction(QObject* parent)
    : QObject(*(new UiActionPrivate), parent)
{
    Q_D(UiAction);
    d->group = qobject_cast<UiActionGroup *>(parent);
    if (d->group)
        d->group->addAction(this);
}

/*!
    Constructs an action with some \a text and \a parent. If \a
    parent is an action group the action will be automatically
    inserted into the group.

    The action uses a stripped version of \a text (e.g. "\&Menu
    Option..." becomes "Menu Option") as descriptive text for
    tool buttons. You can override this by setting a specific
    description with setText(). The same text will be used for
    tooltips unless you specify a different text using
    setToolTip().

*/
UiAction::UiAction(const QString &text, QObject* parent)
    : QObject(*(new UiActionPrivate), parent)
{
    Q_D(UiAction);
    d->text = text;
    d->group = qobject_cast<UiActionGroup *>(parent);
    if (d->group)
        d->group->addAction(this);
}

/*!
    \internal
*/
UiAction::UiAction(UiActionPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(UiAction);
    d->group = qobject_cast<UiActionGroup *>(parent);
    if (d->group)
        d->group->addAction(this);
}

#ifndef QT_NO_SHORTCUT
/*!
    \property QAction::shortcut
    \brief the action's primary shortcut key

    Valid keycodes for this property can be found in \l Qt::Key and
    \l Qt::Modifier. There is no default shortcut key.
*/
void UiAction::setShortcut(const QKeySequence &shortcut)
{
    QAPP_CHECK("setShortcut");

    Q_D(UiAction);
    if (d->shortcut == shortcut)
        return;

    d->shortcut = shortcut;
    d->redoGrab(qApp->d_func()->shortcutMap);
    d->sendDataChanged();
}

/*!
    \since 4.2

    Sets \a shortcuts as the list of shortcuts that trigger the
    action. The first element of the list is the primary shortcut.

    \sa shortcut
*/
void UiAction::setShortcuts(const QList<QKeySequence> &shortcuts)
{
    Q_D(UiAction);

    QList <QKeySequence> listCopy = shortcuts;

    QKeySequence primary;
    if (!listCopy.isEmpty())
        primary = listCopy.takeFirst();

    if (d->shortcut == primary && d->alternateShortcuts == listCopy)
        return;

    QAPP_CHECK("setShortcuts");

    d->shortcut = primary;
    d->alternateShortcuts = listCopy;
    d->redoGrab(qApp->d_func()->shortcutMap);
    d->redoGrabAlternate(qApp->d_func()->shortcutMap);
    d->sendDataChanged();
}

/*!
    \since 4.2

    Sets a platform dependent list of shortcuts based on the \a key.
    The result of calling this function will depend on the currently running platform.
    Note that more than one shortcut can assigned by this action.
    If only the primary shortcut is required, use setShortcut instead.

    \sa QKeySequence::keyBindings()
*/
void UiAction::setShortcuts(QKeySequence::StandardKey key)
{
    QList <QKeySequence> list = QKeySequence::keyBindings(key);
    setShortcuts(list);
}

/*!
    Returns the primary shortcut.

    \sa setShortcuts()
*/
QKeySequence UiAction::shortcut() const
{
    Q_D(const UiAction);
    return d->shortcut;
}

/*!
    \since 4.2

    Returns the list of shortcuts, with the primary shortcut as
    the first element of the list.

    \sa setShortcuts()
*/
QList<QKeySequence> UiAction::shortcuts() const
{
    Q_D(const UiAction);
    QList <QKeySequence> shortcuts;
    if (!d->shortcut.isEmpty())
        shortcuts << d->shortcut;
    if (!d->alternateShortcuts.isEmpty())
        shortcuts << d->alternateShortcuts;
    return shortcuts;
}

/*!
    \property QAction::shortcutContext
    \brief the context for the action's shortcut

    Valid values for this property can be found in \l Qt::ShortcutContext.
    The default value is Qt::WindowShortcut.
*/
void UiAction::setShortcutContext(Qt::ShortcutContext context)
{
    Q_D(UiAction);
    if (d->shortcutContext == context)
        return;
    QAPP_CHECK("setShortcutContext");
    d->shortcutContext = context;
    d->redoGrab(qApp->d_func()->shortcutMap);
    d->redoGrabAlternate(qApp->d_func()->shortcutMap);
    d->sendDataChanged();
}

Qt::ShortcutContext UiAction::shortcutContext() const
{
    Q_D(const UiAction);
    return d->shortcutContext;
}

/*!
    \property QAction::autoRepeat
    \brief whether the action can auto repeat
    \since 4.2

    If true, the action will auto repeat when the keyboard shortcut
    combination is held down, provided that keyboard auto repeat is
    enabled on the system.
    The default value is true.
*/
void UiAction::setAutoRepeat(bool on)
{
    Q_D(UiAction);
    if (d->autorepeat == on)
        return;
    QAPP_CHECK("setAutoRepeat");
    d->autorepeat = on;
    d->redoGrab(qApp->d_func()->shortcutMap);
    d->redoGrabAlternate(qApp->d_func()->shortcutMap);
    d->sendDataChanged();
}

bool UiAction::autoRepeat() const
{
    Q_D(const UiAction);
    return d->autorepeat;
}
#endif // QT_NO_SHORTCUT


/*!
    Destroys the object and frees allocated resources.
*/
UiAction::~UiAction()
{
    Q_D(UiAction);
    if (d->group)
        d->group->removeAction(this);
#ifndef QT_NO_SHORTCUT
    if (d->shortcutId && qApp) {
        qApp->d_func()->shortcutMap.removeShortcut(d->shortcutId, this);
        for (int i = 0; i < d->alternateShortcutIds.count(); ++i) {
            const int id = d->alternateShortcutIds.at(i);
            qApp->d_func()->shortcutMap.removeShortcut(id, this);
        }
    }
#endif
}

/*!
  Sets this action group to \a group. The action will be automatically
  added to the group's list of actions.

  Actions within the group will be mutually exclusive.

  \sa QActionGroup, QAction::actionGroup()
*/
void UiAction::setActionGroup(UiActionGroup *group)
{
    Q_D(UiAction);
    if (group == d->group)
        return;

    if (d->group)
        d->group->removeAction(this);
    d->group = group;
    if (group)
        group->addAction(this);
}

/*!
  Returns the action group for this action. If no action group manages
  this action then 0 will be returned.

  \sa QActionGroup, QAction::setActionGroup()
*/
UiActionGroup *UiAction::actionGroup() const
{
    Q_D(const UiAction);
    return d->group;
}

/*!
    \property QAction::text
    \brief the action's descriptive text

    If the action is added to a menu, the menu option will consist of
    the icon (if there is one), the text, and the shortcut (if there
    is one). If the text is not explicitly set in the constructor, or
    by using setText(), the action's description icon text will be
    used as text. There is no default text.

    \sa iconText
*/
void UiAction::setText(const QString &text)
{
    Q_D(UiAction);
    if (d->text == text)
        return;

    d->text = text;
    d->sendDataChanged();
}

QString UiAction::text() const
{
    Q_D(const UiAction);
    QString s = d->text;
    if (s.isEmpty()) {
        s = d->iconText;
        s.replace(QLatin1Char('&'), QLatin1String("&&"));
    }
    return s;
}

/*!
    \property UiAction::iconText
    \brief the action's descriptive icon text

    If QToolBar::toolButtonStyle is set to a value that permits text to
    be displayed, the text defined held in this property appears as a
    label in the relevant tool button.

    It also serves as the default text in menus and tooltips if the action
    has not been defined with setText() or setToolTip(), and will
    also be used in toolbar buttons if no icon has been defined using setIcon().

    If the icon text is not explicitly set, the action's normal text will be
    used for the icon text.

    By default, this property contains an empty string.

    \sa setToolTip(), setStatusTip()
*/
void UiAction::setIconText(const QString &text)
{
    Q_D(UiAction);
    if (d->iconText == text)
        return;

    d->iconText = text;
    d->sendDataChanged();
}

QString UiAction::iconText() const
{
    Q_D(const UiAction);
    if (d->iconText.isEmpty())
        return qt_strippedText(d->text);
    return d->iconText;
}

/*!
    \property UiAction::toolTip
    \brief the action's tooltip

    This text is used for the tooltip. If no tooltip is specified,
    the action's text is used.

    By default, this property contains the action's text.

    \sa setStatusTip() setShortcut()
*/
void UiAction::setToolTip(const QString &tooltip)
{
    Q_D(UiAction);
    if (d->tooltip == tooltip)
        return;

    d->tooltip = tooltip;
    d->sendDataChanged();
}

QString UiAction::toolTip() const
{
    Q_D(const UiAction);
    if (d->tooltip.isEmpty()) {
        if (!d->text.isEmpty())
            return qt_strippedText(d->text);
        return qt_strippedText(d->iconText);
    }
    return d->tooltip;
}

/*!
    \property UiAction::statusTip
    \brief the action's status tip

    The status tip is displayed on all status bars provided by the
    action's top-level parent widget.

    By default, this property contains an empty string.

    \sa setToolTip() showStatusText()
*/
void UiAction::setStatusTip(const QString &statustip)
{
    Q_D(UiAction);
    if (d->statustip == statustip)
        return;

    d->statustip = statustip;
    d->sendDataChanged();
}

QString UiAction::statusTip() const
{
    Q_D(const UiAction);
    return d->statustip;
}

/*!
    \property UiAction::whatsThis
    \brief the action's "What's This?" help text

    The "What's This?" text is used to provide a brief description of
    the action. The text may contain rich text. There is no default
    "What's This?" text.

    \sa QWhatsThis Q3StyleSheet
*/
void UiAction::setWhatsThis(const QString &whatsthis)
{
    Q_D(UiAction);
    if (d->whatsthis == whatsthis)
        return;

    d->whatsthis = whatsthis;
    d->sendDataChanged();
}

QString UiAction::whatsThis() const
{
    Q_D(const UiAction);
    return d->whatsthis;
}

/*!
    \property QAction::checkable
    \brief whether the action is a checkable action

    A checkable action is one which has an on/off state. For example,
    in a word processor, a Bold toolbar button may be either on or
    off. An action which is not a toggle action is a command action;
    a command action is simply executed, e.g. file save.
    By default, this property is false.

    In some situations, the state of one toggle action should depend
    on the state of others. For example, "Left Align", "Center" and
    "Right Align" toggle actions are mutually exclusive. To achieve
    exclusive toggling, add the relevant toggle actions to a
    QActionGroup with the QActionGroup::exclusive property set to
    true.

    \sa QAction::setChecked()
*/
void UiAction::setCheckable(bool b)
{
    Q_D(UiAction);
    if (d->checkable == b)
        return;

    d->checkable = b;
    d->checked = false;
    d->sendDataChanged();
}

bool UiAction::isCheckable() const
{
    Q_D(const UiAction);
    return d->checkable;
}

/*!
    \fn void QAction::toggle()

    This is a convenience function for the \l checked property.
    Connect to it to change the checked state to its opposite state.
*/
void UiAction::toggle()
{
    Q_D(UiAction);
    setChecked(!d->checked);
}

/*!
    \property QAction::checked
    \brief whether the action is checked.

    Only checkable actions can be checked.  By default, this is false
    (the action is unchecked).

    \sa checkable
*/
void UiAction::setChecked(bool b)
{
    Q_D(UiAction);
    if (!d->checkable || d->checked == b)
        return;

    QPointer<UiAction> guard(this);
    d->checked = b;
    d->sendDataChanged();
    if (guard)
        emit toggled(b);
}

bool UiAction::isChecked() const
{
    Q_D(const UiAction);
    return d->checked;
}

/*!
    \fn void QAction::setDisabled(bool b)

    This is a convenience function for the \l enabled property, that
    is useful for signals--slots connections. If \a b is true the
    action is disabled; otherwise it is enabled.
*/

/*!
    \property QAction::enabled
    \brief whether the action is enabled

    Disabled actions cannot be chosen by the user. They do not
    disappear from menus or toolbars, but they are displayed in a way
    which indicates that they are unavailable. For example, they might
    be displayed using only shades of gray.

    \gui{What's This?} help on disabled actions is still available, provided
    that the QAction::whatsThis property is set.

    An action will be disabled when all widgets to which it is added
    (with QWidget::addAction()) are disabled or not visible. When an
    action is disabled, it is not possible to trigger it through its
    shortcut.

    By default, this property is true (actions are enabled).

    \sa text
*/
void UiAction::setEnabled(bool b)
{
    Q_D(UiAction);
    if (b == d->enabled && b != d->forceDisabled)
        return;
    d->forceDisabled = !b;
    if (b && (!d->visible || (d->group && !d->group->isEnabled())))
        return;
    QAPP_CHECK("setEnabled");
    d->enabled = b;
#ifndef QT_NO_SHORTCUT
    d->setShortcutEnabled(b, qApp->d_func()->shortcutMap);
#endif
    d->sendDataChanged();
}

bool UiAction::isEnabled() const
{
    Q_D(const UiAction);
    return d->enabled;
}

/*!
    \property QAction::visible
    \brief whether the action can be seen (e.g. in menus and toolbars)

    If \e visible is true the action can be seen (e.g. in menus and
    toolbars) and chosen by the user; if \e visible is false the
    action cannot be seen or chosen by the user.

    Actions which are not visible are \e not grayed out; they do not
    appear at all.

    By default, this property is true (actions are visible).
*/
void UiAction::setVisible(bool b)
{
    Q_D(UiAction);
    if (b == d->visible && b != d->forceInvisible)
        return;
    QAPP_CHECK("setVisible");
    d->forceInvisible = !b;
    d->visible = b;
    d->enabled = b && !d->forceDisabled && (!d->group || d->group->isEnabled()) ;
#ifndef QT_NO_SHORTCUT
    d->setShortcutEnabled(d->enabled, qApp->d_func()->shortcutMap);
#endif
    d->sendDataChanged();
}


bool UiAction::isVisible() const
{
    Q_D(const UiAction);
    return d->visible;
}

/*!
  \reimp
*/
bool
UiAction::event(QEvent *e)
{
#ifndef QT_NO_SHORTCUT
    if (e->type() == QEvent::Shortcut) {
        QShortcutEvent *se = static_cast<QShortcutEvent *>(e);
        Q_ASSERT_X(se->key() == d_func()->shortcut || d_func()->alternateShortcuts.contains(se->key()),
                   "UiAction::event",
                   "Received shortcut event from incorrect shortcut");
        if (se->isAmbiguous())
            qWarning("QAction::eventFilter: Ambiguous shortcut overload: %s", se->key().toString(QKeySequence::NativeText).toLatin1().constData());
        else
            activate(Trigger);
        return true;
    }
#endif
    return QObject::event(e);
}

/*!
  Returns the user data as set in QAction::setData.

  \sa setData()
*/
QVariant
UiAction::data() const
{
    Q_D(const UiAction);
    return d->userData;
}

/*!
  \fn void QAction::setData(const QVariant &userData)

  Sets the action's internal data to the given \a userData.

  \sa data()
*/
void
UiAction::setData(const QVariant &data)
{
    Q_D(UiAction);
    d->userData = data;
    d->sendDataChanged();
}


/*!
  Sends the relevant signals for ActionEvent \a event.

  Action based widgets use this API to cause the QAction
  to emit signals as well as emitting their own.
*/
void UiAction::activate(ActionEvent event)
{
    Q_D(UiAction);
    if (event == Trigger) {
        QWeakPointer<QObject> guard = this;
        if (d->checkable) {
            // the checked action of an exclusive group cannot be  unchecked
            if (d->checked && (d->group && d->group->isExclusive()
                               && d->group->checkedAction() == this)) {
                if (!guard.isNull())
                    emit triggered(true);
                return;
            }
            setChecked(!d->checked);
        }
        if (!guard.isNull())
            emit triggered(d->checked);
    }
}

/*!
    \fn void QAction::triggered(bool checked)

    This signal is emitted when an action is activated by the user;
    for example, when the user clicks a menu option, toolbar button,
    or presses an action's shortcut key combination, or when trigger()
    was called. Notably, it is \e not emitted when setChecked() or
    toggle() is called.

    If the action is checkable, \a checked is true if the action is
    checked, or false if the action is unchecked.

    \sa QAction::activate(), QAction::toggled(), checked
*/

/*!
    \fn void QAction::toggled(bool checked)

    This signal is emitted whenever a checkable action changes its
    isChecked() status. This can be the result of a user interaction,
    or because setChecked() was called.

    \a checked is true if the action is checked, or false if the
    action is unchecked.

    \sa QAction::activate(), QAction::triggered(), checked
*/

/*!
    \fn void QAction::changed()

    This signal is emitted when an action has changed. If you
    are only interested in actions in a given widget, you can
    watch for QWidget::actionEvent() sent with an
    QEvent::ActionChanged.

    \sa QWidget::actionEvent()
*/

/*!
    \enum QAction::ActionEvent

    This enum type is used when calling QAction::activate()

    \value Trigger this will cause the QAction::triggered() signal to be emitted.

    \value Hover this will cause the QAction::hovered() signal to be emitted.
*/

QT_END_NAMESPACE_UIHELPERS

#include "moc_uiaction.cpp"

#endif // QT_NO_ACTION
