/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT)
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

#ifndef UICOMPLETIONMODEL_P_H
#define UICOMPLETIONMODEL_P_H

#ifndef QT_NO_COMPLETIONMODEL

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qobject_p.h"

#include "uicompletionmodel.h"
#include "private/qabstractproxymodel_p.h"
#include "QtCore/qstringlist.h"

QT_BEGIN_NAMESPACE_UIHELPERS

class QIndexMapper
{
public:
    QIndexMapper() : v(false), f(0), t(-1) { }
    QIndexMapper(int f, int t) : v(false), f(f), t(t) { }
    QIndexMapper(QVector<int> vec) : v(true), vector(vec), f(-1), t(-1) { }

    inline int count() const { return v ? vector.count() : t - f + 1; }
    inline int operator[] (int index) const { return v ? vector[index] : f + index; }
    inline int indexOf(int x) const { return v ? vector.indexOf(x) : ((t < f) ? -1 : x - f); }
    inline bool isValid() const { return !isEmpty(); }
    inline bool isEmpty() const { return v ? vector.isEmpty() : (t < f); }
    inline void append(int x) { Q_ASSERT(v); vector.append(x); }
    inline int first() const { return v ? vector.first() : f; }
    inline int last() const { return v ? vector.last() : t; }
    inline int from() const { Q_ASSERT(!v); return f; }
    inline int to() const { Q_ASSERT(!v); return t; }
    inline int cost() const { return vector.count()+2; }

private:
    bool v;
    QVector<int> vector;
    int f, t;
};


struct QMatchData {
    QMatchData() : exactMatchIndex(-1) { }
    QMatchData(const QIndexMapper& indices, int em, bool p) :
        indices(indices), exactMatchIndex(em), partial(p) { }
    QIndexMapper indices;
    inline bool isValid() const { return indices.isValid(); }
    int  exactMatchIndex;
    bool partial;
};


class UiCompletionEngine
{
public:
    typedef QMap<QString, QMatchData> CacheItem;
    typedef QMap<QModelIndex, CacheItem> Cache;

    UiCompletionEngine(UiCompletionModelPrivate *c) : c(c), curRow(-1), cost(0) { }
    virtual ~UiCompletionEngine() { }

    void filter(const QStringList &parts);

    QMatchData filterHistory();
    bool matchHint(QString, const QModelIndex&, QMatchData*);

    void saveInCache(QString, const QModelIndex&, const QMatchData&);
    bool lookupCache(QString part, const QModelIndex& parent, QMatchData *m);

    virtual void filterOnDemand(int) { }
    virtual QMatchData filter(const QString&, const QModelIndex&, int) = 0;

    int matchCount() const { return curMatch.indices.count() + historyMatch.indices.count(); }

    QMatchData curMatch, historyMatch;
    UiCompletionModelPrivate *c;
    QStringList curParts;
    QModelIndex curParent;
    int curRow;

    Cache cache;
    int cost;
};


class QSortedModelEngine : public UiCompletionEngine
{
public:
    QSortedModelEngine(UiCompletionModelPrivate *c) : UiCompletionEngine(c) { }
    QMatchData filter(const QString&, const QModelIndex&, int);
    QIndexMapper indexHint(QString, const QModelIndex&, Qt::SortOrder);
    Qt::SortOrder sortOrder(const QModelIndex&) const;
};


class QUnsortedModelEngine : public UiCompletionEngine
{
public:
    QUnsortedModelEngine(UiCompletionModelPrivate *c) : UiCompletionEngine(c) { }

    void filterOnDemand(int);
    QMatchData filter(const QString&, const QModelIndex&, int);
private:
    int buildIndices(const QString& str, const QModelIndex& parent, int n,
                     const QIndexMapper& iv, QMatchData* m);
};


class UiCompletionModelPrivate : public QAbstractProxyModelPrivate
{
    Q_DECLARE_PUBLIC(UiCompletionModel)

public:
    UiCompletionModelPrivate(UiCompletionModel *model) :
        proxy(model), showAll(false), cs(Qt::CaseSensitive), role(Qt::EditRole), column(0), sorting(UiCompletionModel::UnsortedModel) { }

    UiCompletionModel *proxy;
    bool showAll;
    QString prefix;
    Qt::CaseSensitivity cs;
    int role;
    int column;
    UiCompletionModel::ModelSorting sorting;
    QScopedPointer<UiCompletionEngine> engine;
};

QT_END_NAMESPACE_UIHELPERS

#endif // QT_NO_COMPLETIONMODEL

#endif // UICOMPLETIONMODEL_P_H
