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


#include <QtTest/QtTest>
#include <UiHelpers/UiUndoStack>

QT_USE_NAMESPACE_UIHELPERS

/******************************************************************************
** Commands
*/

class InsertCommand : public UiUndoCommand
{
public:
    InsertCommand(QString *str, int idx, const QString &text,
                    UiUndoCommand *parent = 0);

    virtual void undo();
    virtual void redo();

private:
    QString *m_str;
    int m_idx;
    QString m_text;
};

class RemoveCommand : public UiUndoCommand
{
public:
    RemoveCommand(QString *str, int idx, int len, UiUndoCommand *parent = 0);

    virtual void undo();
    virtual void redo();

private:
    QString *m_str;
    int m_idx;
    QString m_text;
};

class AppendCommand : public UiUndoCommand
{
public:
    AppendCommand(QString *str, const QString &text, bool _fail_merge = false,
                    UiUndoCommand *parent = 0);
    ~AppendCommand();

    virtual void undo();
    virtual void redo();
    virtual int id() const;
    virtual bool mergeWith(const UiUndoCommand *other);

    bool merged;
    bool fail_merge;
    static int delete_cnt;

private:
    QString *m_str;
    QString m_text;
};

class IdleCommand : public UiUndoCommand
{
public:
    IdleCommand(UiUndoCommand *parent = 0);
    ~IdleCommand();

    virtual void undo();
    virtual void redo();
};

InsertCommand::InsertCommand(QString *str, int idx, const QString &text,
                            UiUndoCommand *parent)
    : UiUndoCommand(parent)
{
    QVERIFY(str->length() >= idx);

    setText("insert");

    m_str = str;
    m_idx = idx;
    m_text = text;
}

void InsertCommand::redo()
{
    QVERIFY(m_str->length() >= m_idx);

    m_str->insert(m_idx, m_text);
}

void InsertCommand::undo()
{
    QCOMPARE(m_str->mid(m_idx, m_text.length()), m_text);

    m_str->remove(m_idx, m_text.length());
}

RemoveCommand::RemoveCommand(QString *str, int idx, int len, UiUndoCommand *parent)
    : UiUndoCommand(parent)
{
    QVERIFY(str->length() >= idx + len);

    setText("remove");

    m_str = str;
    m_idx = idx;
    m_text = m_str->mid(m_idx, len);
}

void RemoveCommand::redo()
{
    QCOMPARE(m_str->mid(m_idx, m_text.length()), m_text);

    m_str->remove(m_idx, m_text.length());
}

void RemoveCommand::undo()
{
    QVERIFY(m_str->length() >= m_idx);

    m_str->insert(m_idx, m_text);
}

int AppendCommand::delete_cnt = 0;

AppendCommand::AppendCommand(QString *str, const QString &text, bool _fail_merge,
                                UiUndoCommand *parent)
    : UiUndoCommand(parent)
{
    setText("append");

    m_str = str;
    m_text = text;
    merged = false;
    fail_merge = _fail_merge;
}

AppendCommand::~AppendCommand()
{
    ++delete_cnt;
}

void AppendCommand::redo()
{
    m_str->append(m_text);
}

void AppendCommand::undo()
{
    QCOMPARE(m_str->mid(m_str->length() - m_text.length()), m_text);

    m_str->truncate(m_str->length() - m_text.length());
}

int AppendCommand::id() const
{
    return 1;
}

bool AppendCommand::mergeWith(const UiUndoCommand *other)
{
    if (other->id() != id())
        return false;
    if (fail_merge)
        return false;
    m_text += static_cast<const AppendCommand*>(other)->m_text;
    merged = true;
    return true;
}

IdleCommand::IdleCommand(UiUndoCommand *parent)
    : UiUndoCommand(parent)
{
    // "idle-item" goes to UiUndoStack::{redo,undo}Text
    // "idle-action" goes to all other places (e.g. UiUndoView)
    setText("idle-item\nidle-action");
}

IdleCommand::~IdleCommand()
{
}

void IdleCommand::redo()
{
}

void IdleCommand::undo()
{
}

/******************************************************************************
** tst_UiUndoStack
*/

class tst_UiUndoStack : public QObject
{
    Q_OBJECT
public:
    tst_UiUndoStack();

private slots:
    void undoRedo();
    void setIndex();
    void setClean();
    void clear();
    void childCommand();
    void macroBeginEnd();
    void compression();
    void undoLimit();
    void separateUndoText();
};

tst_UiUndoStack::tst_UiUndoStack()
{
}

static QString glue(const QString &s1, const QString &s2)
{
    QString result;

    result.append(s1);
    if (!s1.isEmpty() && !s2.isEmpty())
        result.append(' ');
    result.append(s2);

    return result;
}

static void checkState(QSignalSpy &redoTextChangedSpy,
                       QSignalSpy &canRedoChangedSpy,
                       QSignalSpy &undoTextChangedSpy,
                       QSignalSpy &canUndoChangedSpy,
                       QSignalSpy &cleanChangedSpy,
                       QSignalSpy &indexChangedSpy,
                       UiUndoStack &stack,
                       const bool _clean,
                       const int _count,
                       const int _index,
                       const bool _canUndo,
                       const QString &_undoText,
                       const bool _canRedo,
                       const QString &_redoText,
                       const bool _cleanChanged,
                       const bool _indexChanged,
                       const bool _undoChanged,
                       const bool _redoChanged)
{
    QCOMPARE(stack.count(), _count);
    QCOMPARE(stack.isClean(), _clean);
    QCOMPARE(stack.index(), _index);
    QCOMPARE(stack.canUndo(), _canUndo);
    QCOMPARE(stack.undoText(), QString(_undoText));
    QCOMPARE(stack.canRedo(), _canRedo);
    QCOMPARE(stack.redoText(), QString(_redoText));
    if (_indexChanged) {
        QCOMPARE(indexChangedSpy.count(), 1);
        QCOMPARE(indexChangedSpy.at(0).at(0).toInt(), _index);
        indexChangedSpy.clear();
    } else {
        QCOMPARE(indexChangedSpy.count(), 0);
    }
    if (_cleanChanged) {
        QCOMPARE(cleanChangedSpy.count(), 1);
        QCOMPARE(cleanChangedSpy.at(0).at(0).toBool(), _clean);
        cleanChangedSpy.clear();
    } else {
        QCOMPARE(cleanChangedSpy.count(), 0);
    }
    if (_undoChanged) {
        QCOMPARE(canUndoChangedSpy.count(), 1);
        QCOMPARE(canUndoChangedSpy.at(0).at(0).toBool(), _canUndo);
        QCOMPARE(undoTextChangedSpy.count(), 1);
        QCOMPARE(undoTextChangedSpy.at(0).at(0).toString(), QString(_undoText));
        canUndoChangedSpy.clear();
        undoTextChangedSpy.clear();
    } else {
        QCOMPARE(canUndoChangedSpy.count(), 0);
        QCOMPARE(undoTextChangedSpy.count(), 0);
    }
    if (_redoChanged) {
        QCOMPARE(canRedoChangedSpy.count(), 1);
        QCOMPARE(canRedoChangedSpy.at(0).at(0).toBool(), _canRedo);
        QCOMPARE(redoTextChangedSpy.count(), 1);
        QCOMPARE(redoTextChangedSpy.at(0).at(0).toString(), QString(_redoText));
        canRedoChangedSpy.clear();
        redoTextChangedSpy.clear();
    } else {
        QCOMPARE(canRedoChangedSpy.count(), 0);
        QCOMPARE(redoTextChangedSpy.count(), 0);
    }
}

void tst_UiUndoStack::undoRedo()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    // push, undo, redo

    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.undo(); // nothing to undo
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new InsertCommand(&str, 0, "hello"));
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 2, "123"));
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged


    stack.undo();
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.redo();
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.redo(); // nothing to redo
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.undo();
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo(); // nothing to undo
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    // push after undo - check that undone commands get deleted

    stack.redo();
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new RemoveCommand(&str, 2, 2));
    QCOMPARE(str, QString("heo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count - still 2, last command got deleted
                2,          // index
                true,       // canUndo
                "remove",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "remove",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 0, "goodbye"));
    QCOMPARE(str, QString("goodbye"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count - two commands got deleted
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::setIndex()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    stack.setIndex(10); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.setIndex(0); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.setIndex(-10); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new InsertCommand(&str, 0, "hello"));
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 2, "123"));
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(2);
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.setIndex(0);
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(10); // should set index to 2
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(-10); // should set index to 0
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(1);
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(2);
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::setClean()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    QCOMPARE(stack.cleanIndex(), 0);
    stack.setClean();
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged
    QCOMPARE(stack.cleanIndex(), 0);

    stack.push(new InsertCommand(&str, 0, "goodbye"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), 0);

    stack.setClean();
    QCOMPARE(str, QString("goodbye"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged
    QCOMPARE(stack.cleanIndex(), 1);

    stack.push(new AppendCommand(&str, " cowboy"));
    QCOMPARE(str, QString("goodbye cowboy"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), 1);

    stack.undo(); // reaching clean state from above
    QCOMPARE(str, QString("goodbye"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "append",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), 1);

    stack.undo();
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), 1);

    stack.redo(); // reaching clean state from below
    QCOMPARE(str, QString("goodbye"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "append",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), 1);

    stack.undo();
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), 1);

    stack.push(new InsertCommand(&str, 0, "foo")); // the clean state gets deleted!
    QCOMPARE(str, QString("foo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), -1);

    stack.undo();
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
    QCOMPARE(stack.cleanIndex(), -1);
}

void tst_UiUndoStack::clear()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    stack.clear();
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new InsertCommand(&str, 0, "hello"));
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 2, "123"));
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.clear();
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    str.clear();
    stack.push(new InsertCommand(&str, 0, "hello"));
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 2, "123"));
    QCOMPARE(str, QString("he123llo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(0);
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,      // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "insert",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.clear();
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                0,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::childCommand()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    stack.push(new InsertCommand(&str, 0, "hello"));
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    UiUndoCommand *cmd = new UiUndoCommand();
    cmd->setText("ding");
    new InsertCommand(&str, 5, "world", cmd);
    new RemoveCommand(&str, 4, 1, cmd);
    stack.push(cmd);
    QCOMPARE(str, QString("hellworld"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "ding",     // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "ding",     // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.redo();
    QCOMPARE(str, QString("hellworld"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "ding",     // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::macroBeginEnd()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    stack.beginMacro("ding");
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setClean(); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.undo(); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.redo(); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.setIndex(0); // should do nothing
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.endMacro();
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index - endMacro() increments index
                true,       // canUndo
                "ding",     // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 0, "h"));
    QCOMPARE(str, QString("h"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 1, "owdy"));
    QCOMPARE(str, QString("howdy"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(2);
    QCOMPARE(str, QString("h"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.beginMacro("dong"); // the "owdy" command gets deleted
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new InsertCommand(&str, 1, "ello"));
    QCOMPARE(str, QString("hello"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new RemoveCommand(&str, 1, 2));
    QCOMPARE(str, QString("hlo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.beginMacro("dong2");
    QCOMPARE(str, QString("hlo"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new RemoveCommand(&str, 1, 1));
    QCOMPARE(str, QString("ho"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.endMacro();
    QCOMPARE(str, QString("ho"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.endMacro();
    QCOMPARE(str, QString("ho"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "dong",     // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("h"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                true,       // canUndo
                "insert",     // undoText
                true,       // canRedo
                "dong",     // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString(""));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                1,          // index
                true,       // canUndo
                "ding",     // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(3);
    QCOMPARE(str, QString("ho"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "dong",     // undoText
                false,      // canRedo
                "",         // redoText
                false,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setIndex(1);
    QCOMPARE(str, QString());
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                1,          // index
                true,       // canUndo
                "ding",     // undoText
                true,       // canRedo
                "insert",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::compression()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    QString str;

    AppendCommand::delete_cnt = 0;

    stack.push(new InsertCommand(&str, 0, "ene"));
    QCOMPARE(str, QString("ene"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, " due")); // #1
    QCOMPARE(str, QString("ene due"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, " rike")); // #2 should merge
    QCOMPARE(str, QString("ene due rike"));
    QCOMPARE(AppendCommand::delete_cnt, 1); // #2 should be deleted
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setClean();
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new AppendCommand(&str, " fake")); // #3 should NOT merge, since the stack was clean
    QCOMPARE(str, QString("ene due rike fake"));  // and we want to be able to return to this state
    QCOMPARE(AppendCommand::delete_cnt, 1); // #3 should not be deleted
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("ene due rike"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                3,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "append",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("ene"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                1,          // index
                true,       // canUndo
                "insert",   // undoText
                true,       // canRedo
                "append",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "ma", true)); // #4 clean state gets deleted!
    QCOMPARE(str, QString("enema"));
    QCOMPARE(AppendCommand::delete_cnt, 3); // #1 got deleted
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "trix")); // #5 should NOT merge
    QCOMPARE(str, QString("enematrix"));
    QCOMPARE(AppendCommand::delete_cnt, 3);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("enema"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "append",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    // and now for command compression inside macros

    stack.setClean();
    QCOMPARE(str, QString("enema"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                3,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "append",   // redoText
                true,       // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.beginMacro("ding");
    QCOMPARE(str, QString("enema"));
    QCOMPARE(AppendCommand::delete_cnt, 4); // #5 gets deleted
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    AppendCommand *merge_cmd = new AppendCommand(&str, "top");
    stack.push(merge_cmd); // #6
    QCOMPARE(merge_cmd->merged, false);
    QCOMPARE(str, QString("enematop"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new AppendCommand(&str, "eja")); // #7 should merge
    QCOMPARE(str, QString("enematopeja"));
    QCOMPARE(merge_cmd->merged, true);
    QCOMPARE(AppendCommand::delete_cnt, 5); // #7 gets deleted
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged
    merge_cmd->merged = false;

    stack.push(new InsertCommand(&str, 2, "123")); // should not merge
    QCOMPARE(str, QString("en123ematopeja"));
    QCOMPARE(merge_cmd->merged, false);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.endMacro();
    QCOMPARE(str, QString("en123ematopeja"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "ding",     // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("enema"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,      // clean
                3,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "ding",     // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.redo();
    QCOMPARE(str, QString("en123ematopeja"));
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                3,          // index
                true,       // canUndo
                "ding",     // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::undoLimit()
{
    UiUndoStack stack;
    QSignalSpy indexChangedSpy(&stack, SIGNAL(indexChanged(int)));
    QSignalSpy cleanChangedSpy(&stack, SIGNAL(cleanChanged(bool)));
    QSignalSpy canUndoChangedSpy(&stack, SIGNAL(canUndoChanged(bool)));
    QSignalSpy undoTextChangedSpy(&stack, SIGNAL(undoTextChanged(QString)));
    QSignalSpy canRedoChangedSpy(&stack, SIGNAL(canRedoChanged(bool)));
    QSignalSpy redoTextChangedSpy(&stack, SIGNAL(redoTextChanged(QString)));
    AppendCommand::delete_cnt = 0;
    QString str;

    QCOMPARE(stack.undoLimit(), 0);
    stack.setUndoLimit(2);
    QCOMPARE(stack.undoLimit(), 2);

    stack.push(new AppendCommand(&str, "1", true));
    QCOMPARE(str, QString("1"));
    QCOMPARE(AppendCommand::delete_cnt, 0);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "2", true));
    QCOMPARE(str, QString("12"));
    QCOMPARE(AppendCommand::delete_cnt, 0);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.setClean();
    QCOMPARE(str, QString("12"));
    QCOMPARE(AppendCommand::delete_cnt, 0);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new AppendCommand(&str, "3", true));
    QCOMPARE(str, QString("123"));
    QCOMPARE(AppendCommand::delete_cnt, 1);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "4", true));
    QCOMPARE(str, QString("1234"));
    QCOMPARE(AppendCommand::delete_cnt, 2);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("123"));
    QCOMPARE(AppendCommand::delete_cnt, 2);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "append",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("12"));
    QCOMPARE(AppendCommand::delete_cnt, 2);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                true,       // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "append",   // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "3", true));
    QCOMPARE(str, QString("123"));
    QCOMPARE(AppendCommand::delete_cnt, 4);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                true,       // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "4", true));
    QCOMPARE(str, QString("1234"));
    QCOMPARE(AppendCommand::delete_cnt, 4);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "5", true));
    QCOMPARE(str, QString("12345"));
    QCOMPARE(AppendCommand::delete_cnt, 5);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("1234"));
    QCOMPARE(AppendCommand::delete_cnt, 5);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "append",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("123"));
    QCOMPARE(AppendCommand::delete_cnt, 5);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "append",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "4", true));
    QCOMPARE(str, QString("1234"));
    QCOMPARE(AppendCommand::delete_cnt, 7);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                1,          // count
                1,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "5"));
    QCOMPARE(str, QString("12345"));
    QCOMPARE(AppendCommand::delete_cnt, 7);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "6", true)); // should be merged
    QCOMPARE(str, QString("123456"));
    QCOMPARE(AppendCommand::delete_cnt, 8);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "append",   // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.beginMacro("foo");
    QCOMPARE(str, QString("123456"));
    QCOMPARE(AppendCommand::delete_cnt, 8);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.push(new AppendCommand(&str, "7", true));
    QCOMPARE(str, QString("1234567"));
    QCOMPARE(AppendCommand::delete_cnt, 8);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.push(new AppendCommand(&str, "8"));
    QCOMPARE(str, QString("12345678"));
    QCOMPARE(AppendCommand::delete_cnt, 8);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                3,          // count
                2,          // index
                false,      // canUndo
                "",         // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                false,      // indexChanged
                false,      // undoChanged
                false);     // redoChanged

    stack.endMacro();
    QCOMPARE(str, QString("12345678"));
    QCOMPARE(AppendCommand::delete_cnt, 9);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                2,          // index
                true,       // canUndo
                "foo",      // undoText
                false,      // canRedo
                "",         // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("123456"));
    QCOMPARE(AppendCommand::delete_cnt, 9);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                1,          // index
                true,       // canUndo
                "append",   // undoText
                true,       // canRedo
                "foo",      // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged

    stack.undo();
    QCOMPARE(str, QString("1234"));
    QCOMPARE(AppendCommand::delete_cnt, 9);
    checkState(redoTextChangedSpy,
                canRedoChangedSpy,
                undoTextChangedSpy,
                canUndoChangedSpy,
                cleanChangedSpy,
                indexChangedSpy,
                stack,
                false,      // clean
                2,          // count
                0,          // index
                false,      // canUndo
                "",         // undoText
                true,       // canRedo
                "append",   // redoText
                false,      // cleanChanged
                true,       // indexChanged
                true,       // undoChanged
                true);      // redoChanged
}

void tst_UiUndoStack::separateUndoText()
{
    UiUndoStack stack;

    UiUndoCommand *command1 = new IdleCommand();
    UiUndoCommand *command2 = new IdleCommand();
    stack.push(command1);
    stack.push(command2);
    stack.undo();

    QCOMPARE(command1->actionText(), QString("idle-action"));

    QCOMPARE(command1->text(), QString("idle-item"));
    QCOMPARE(stack.text(0), QString("idle-item"));

    command1->setText("idle");
    QCOMPARE(command1->actionText(), QString("idle"));
    QCOMPARE(command1->text(), QString("idle"));

    command1->setText("idle-item\nidle-action");
    QCOMPARE(command1->actionText(), QString("idle-action"));
    QCOMPARE(command1->text(), QString("idle-item"));
}

QTEST_MAIN(tst_UiUndoStack)

#include "tst_uiundostack.moc"
