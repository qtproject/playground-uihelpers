/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>
#include <UiHelpers/UiAction>
#include <UiHelpers/UiActionGroup>

#include <QCoreApplication>

// #include <qevent.h>
// #include <qplatformtheme_qpa.h>
// #include <private/qguiapplication_p.h>

QT_USE_NAMESPACE_UIHELPERS

class tst_UiAction : public QObject
{
    Q_OBJECT

public:
    tst_UiAction();
    virtual ~tst_UiAction();


    void updateState(UiActionEvent *e);

public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void getSetCheck();
    void setText_data();
    void setText();
    void setIconText_data() { setText_data(); }
    void setIconText();
    void actionEvent();
    // void setStandardKeys();
    // void alternateShortcuts();
    // void task200823_tooltip();
    void enabledVisibleInteraction();
    void task229128TriggeredSignalWithoutActiongroup();
    void task229128TriggeredSignalWhenInActiongroup();

private:
    int m_lastEventType;
    int m_keyboardScheme;
    UiAction *m_lastAction;
    QObject *m_tstObject;
};

// Testing get/set functions
void tst_UiAction::getSetCheck()
{
    UiAction obj1(0);
    UiActionGroup *var1 = new UiActionGroup(0);
    obj1.setActionGroup(var1);
    QCOMPARE(var1, obj1.actionGroup());
    obj1.setActionGroup((UiActionGroup *)0);
    QCOMPARE((UiActionGroup *)0, obj1.actionGroup());
    delete var1;
}

class MyObject : public QObject
{
    Q_OBJECT
public:
    MyObject(tst_UiAction *tst, QObject *parent = 0) : QObject(parent), action(0) { this->tst = tst; }
    virtual bool event(QEvent *e);
    void addAction(UiAction *);
    void removeAction(UiAction *);

private:
    tst_UiAction *tst;
    UiAction *action;
};

bool MyObject::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::ActionAdded:
    case QEvent::ActionRemoved:
    case QEvent::ActionChanged:
        tst->updateState((UiActionEvent*)e);
        break;
    default:
        return QObject::event(e);
    }

    return true;
}

void MyObject::addAction(UiAction *action)
{
    if (this->action == action)
        removeAction(action);

    UiAction *before = this->action;
    this->action = action;

    UiActionEvent e(QEvent::ActionAdded, action, before);
    QCoreApplication::sendEvent(this, &e);
}

void MyObject::removeAction(UiAction *action)
{
    if (!action)
        return;

    if (this->action == action) {
        this->action = 0;
        UiActionEvent e(QEvent::ActionRemoved, action);
        QCoreApplication::sendEvent(this, &e);
    }
}

tst_UiAction::tst_UiAction()// : m_keyboardScheme(QPlatformTheme::WindowsKeyboardScheme)
{
    // if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme())
    //     m_keyboardScheme = theme->themeHint(QPlatformTheme::KeyboardScheme).toInt();
}

tst_UiAction::~tst_UiAction()
{

}

void tst_UiAction::initTestCase()
{
    m_lastEventType = 0;
    m_lastAction = 0;

    MyObject *mo = new MyObject(this);
    m_tstObject = mo;
}

void tst_UiAction::cleanupTestCase()
{
    QObject *testObject = m_tstObject;
    if (testObject)
        delete testObject;
}

void tst_UiAction::setText_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("iconText");
    QTest::addColumn<QString>("textFromIconText");

    //next we fill it with data
    QTest::newRow("Normal") << "Action" << "Action" << "Action";
    QTest::newRow("Ampersand") << "Search && Destroy" << "Search & Destroy" << "Search && Destroy";
    QTest::newRow("Mnemonic and ellipsis") << "O&pen File ..." << "Open File" << "Open File";
}

void tst_UiAction::setText()
{
    QFETCH(QString, text);

    UiAction action(0);
    action.setText(text);

    QCOMPARE(action.text(), text);

    QFETCH(QString, iconText);
    QCOMPARE(action.iconText(), iconText);
}

void tst_UiAction::setIconText()
{
    QFETCH(QString, iconText);

    UiAction action(0);
    action.setIconText(iconText);
    QCOMPARE(action.iconText(), iconText);

    QFETCH(QString, textFromIconText);
    QCOMPARE(action.text(), textFromIconText);
}

void tst_UiAction::updateState(UiActionEvent *e)
{
    if (!e) {
        m_lastEventType = 0;
        m_lastAction = 0;
    } else {
        m_lastEventType = (int)e->type();
        m_lastAction = e->action();
    }
}

void tst_UiAction::actionEvent()
{
    UiAction a(0);
    a.setText("action text");

    MyObject *tstMyObject = qobject_cast<MyObject*>(m_tstObject);

    // add action
    tstMyObject->addAction(&a);
    qApp->processEvents();

    QCOMPARE(m_lastEventType, (int)QEvent::ActionAdded);
    QCOMPARE(m_lastAction, &a);

    // change action
    // a.setText("new action text");
    // qApp->processEvents();
    //
    // QCOMPARE(m_lastEventType, (int)QEvent::ActionChanged);
    // QCOMPARE(m_lastAction, &a);

    // remove action
    tstMyObject->removeAction(&a);
    qApp->processEvents();

    QCOMPARE(m_lastEventType, (int)QEvent::ActionRemoved);
    QCOMPARE(m_lastAction, &a);
}

// TODO: Rewrite test after shortcuts in UiAction is written
// //basic testing of standard keys
// void tst_QAction::setStandardKeys()
// {
//     QAction act(0);
//     act.setShortcut(QKeySequence("CTRL+L"));
//     QList<QKeySequence> list;
//     act.setShortcuts(list);
//     act.setShortcuts(QKeySequence::Copy);
//     QVERIFY(act.shortcut() == act.shortcuts().first());
//
//     QList<QKeySequence> expected;
//     const QKeySequence ctrlC = QKeySequence(QStringLiteral("CTRL+C"));
//     const QKeySequence ctrlInsert = QKeySequence(QStringLiteral("CTRL+INSERT"));
//     switch (m_keyboardScheme) {
//     case QPlatformTheme::MacKeyboardScheme:
//         expected  << ctrlC;
//         break;
//     case QPlatformTheme::WindowsKeyboardScheme:
//         expected  << ctrlC << ctrlInsert;
//         break;
//     default: // X11
//         expected  << ctrlC << QKeySequence(QStringLiteral("F16")) << ctrlInsert;
//         break;
//     }
//
//     QCOMPARE(act.shortcuts(), expected);
// }
//
//
// void tst_QAction::alternateShortcuts()
// {
//     //test the alternate shortcuts (by adding more than 1 shortcut)
//
//     QWidget *wid = m_tstWidget;
//
//     {
//         QAction act(wid);
//         wid->addAction(&act);
//         QList<QKeySequence> shlist = QList<QKeySequence>() << QKeySequence("CTRL+P") << QKeySequence("CTRL+A");
//         act.setShortcuts(shlist);
//
//         QSignalSpy spy(&act, SIGNAL(triggered()));
//
//         act.setAutoRepeat(true);
//         QTest::keyClick(wid, Qt::Key_A, Qt::ControlModifier);
//         QCOMPARE(spy.count(), 1); //act should have been triggered
//
//         act.setAutoRepeat(false);
//         QTest::keyClick(wid, Qt::Key_A, Qt::ControlModifier);
//         QCOMPARE(spy.count(), 2); //act should have been triggered a 2nd time
//
//         //end of the scope of the action, it will be destroyed and removed from wid
//         //This action should also unregister its shortcuts
//     }
//
//
//     //this tests a crash (if the action did not unregister its alternate shortcuts)
//     QTest::keyClick(wid, Qt::Key_A, Qt::ControlModifier);
// }

void tst_UiAction::enabledVisibleInteraction()
{
    UiAction act(0);
    // check defaults
    QVERIFY(act.isEnabled());
    QVERIFY(act.isVisible());

    // !visible => !enabled
    act.setVisible(false);
    QVERIFY(!act.isEnabled());
    act.setVisible(true);
    QVERIFY(act.isEnabled());
    act.setEnabled(false);
    QVERIFY(act.isVisible());

//     // check if shortcut is disabled if not visible
//     m_tstWidget->addAction(&act);
//     act.setShortcut(QKeySequence("Ctrl+T"));
//     QSignalSpy spy(&act, SIGNAL(triggered()));
//     act.setEnabled(true);
//     act.setVisible(false);
//     QTest::keyClick(m_tstWidget, Qt::Key_T, Qt::ControlModifier);
//     QCOMPARE(spy.count(), 0); //act is not visible, so don't trigger
//     act.setVisible(false);
//     act.setEnabled(true);
//     QTest::keyClick(m_tstWidget, Qt::Key_T, Qt::ControlModifier);
//     QCOMPARE(spy.count(), 0); //act is not visible, so don't trigger
//     act.setVisible(true);
//     act.setEnabled(true);
//     QTest::keyClick(m_tstWidget, Qt::Key_T, Qt::ControlModifier);
//     QCOMPARE(spy.count(), 1); //act is visible and enabled, so trigger
}

// void tst_QAction::task200823_tooltip()
// {
//     QAction *action = new QAction("foo", 0);
//     QString shortcut("ctrl+o");
//     action->setShortcut(shortcut);
//
//     // we want a non-standard tooltip that shows the shortcut
//     action->setToolTip(QString("%1 (%2)").arg(action->text()).arg(action->shortcut().toString()));
//
//     QString ref = QString("foo (%1)").arg(QKeySequence(shortcut).toString());
//     QCOMPARE(action->toolTip(), ref);
// }

void tst_UiAction::task229128TriggeredSignalWithoutActiongroup()
{
    // test without a group
    UiAction *actionWithoutGroup = new UiAction("Test", qApp);
    QSignalSpy spyWithoutGroup(actionWithoutGroup, SIGNAL(triggered(bool)));
    QCOMPARE(spyWithoutGroup.count(), 0);
    actionWithoutGroup->trigger();
    // signal should be emitted
    QCOMPARE(spyWithoutGroup.count(), 1);

    // it is now a checkable checked action
    actionWithoutGroup->setCheckable(true);
    actionWithoutGroup->setChecked(true);
    spyWithoutGroup.clear();
    QCOMPARE(spyWithoutGroup.count(), 0);
    actionWithoutGroup->trigger();
    // signal should be emitted
    QCOMPARE(spyWithoutGroup.count(), 1);
}

void tst_UiAction::task229128TriggeredSignalWhenInActiongroup()
{
    UiActionGroup ag(0);
    UiAction *action = new UiAction("Test", &ag);
    UiAction *checkedAction = new UiAction("Test 2", &ag);
    ag.addAction(action);
    action->setCheckable(true);
    ag.addAction(checkedAction);
    checkedAction->setCheckable(true);
    checkedAction->setChecked(true);

    QSignalSpy actionSpy(checkedAction, SIGNAL(triggered(bool)));
    qRegisterMetaType<UiAction*>("UiAction*");
    QSignalSpy actionGroupSpy(&ag, SIGNAL(triggered(UiAction *)));
    QCOMPARE(actionGroupSpy.count(), 0);
    QCOMPARE(actionSpy.count(), 0);
    checkedAction->trigger();
    // check that both the group and the action have emitted the signal
    QCOMPARE(actionGroupSpy.count(), 1);
    QCOMPARE(actionSpy.count(), 1);
}

QTEST_MAIN(tst_UiAction)
#include "tst_uiaction.moc"
