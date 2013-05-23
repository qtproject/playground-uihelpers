/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QT_NO_COMPLETIONMODEL

#include "uicompletionmodel_p.h"
#include <QtCore/QStringListModel>
#include <QtCore/QDir>

QT_BEGIN_NAMESPACE_UIHELPERS

UiCompletionModel::UiCompletionModel(QObject *parent)
    : QAbstractProxyModel(*new UiCompletionModelPrivate(this), parent)
{
    createEngine();
}

int UiCompletionModel::columnCount(const QModelIndex &) const
{
    Q_D(const UiCompletionModel);
    return d->model->columnCount();
}

void UiCompletionModel::setSourceModel(QAbstractItemModel *source)
{
    bool hadModel = (sourceModel() != 0);

    if (hadModel)
        QObject::disconnect(sourceModel(), 0, this, 0);

    QAbstractProxyModel::setSourceModel(source);

    if (source) {
        // TODO: Optimize updates in the source model
        connect(source, SIGNAL(modelReset()), this, SLOT(invalidate()));
        connect(source, SIGNAL(destroyed()), this, SLOT(modelDestroyed()));
        connect(source, SIGNAL(layoutChanged()), this, SLOT(invalidate()));
        connect(source, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted()));
        connect(source, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(invalidate()));
        connect(source, SIGNAL(columnsInserted(QModelIndex,int,int)), this, SLOT(invalidate()));
        connect(source, SIGNAL(columnsRemoved(QModelIndex,int,int)), this, SLOT(invalidate()));
        connect(source, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(invalidate()));
    }

    invalidate();
}

void UiCompletionModel::createEngine()
{
    Q_D(UiCompletionModel);

    bool sortedEngine = false;
    switch (d->sorting) {
    case UnsortedModel:
        sortedEngine = false;
        break;
    case CaseSensitivelySortedModel:
        sortedEngine = d->cs == Qt::CaseSensitive;
        break;
    case CaseInsensitivelySortedModel:
        sortedEngine = d->cs == Qt::CaseInsensitive;
        break;
    }

    if (sortedEngine)
        d->engine.reset(new QSortedModelEngine(d));
    else
        d->engine.reset(new QUnsortedModelEngine(d));
}

QModelIndex UiCompletionModel::mapToSource(const QModelIndex& index) const
{
    Q_D(const UiCompletionModel);
    if (!index.isValid())
        return d->engine->curParent;

    int row;
    QModelIndex parent = d->engine->curParent;
    if (!d->showAll) {
        if (!d->engine->matchCount())
            return QModelIndex();
        Q_ASSERT(index.row() < d->engine->matchCount());
        QIndexMapper& rootIndices = d->engine->historyMatch.indices;
        if (index.row() < rootIndices.count()) {
            row = rootIndices[index.row()];
            parent = QModelIndex();
        } else {
            row = d->engine->curMatch.indices[index.row() - rootIndices.count()];
        }
    } else {
        row = index.row();
    }

    return d->model->index(row, index.column(), parent);
}

QModelIndex UiCompletionModel::mapFromSource(const QModelIndex& idx) const
{
    Q_D(const UiCompletionModel);
    if (!idx.isValid())
        return QModelIndex();

    int row = -1;
    if (!d->showAll) {
        if (!d->engine->matchCount())
            return QModelIndex();

        QIndexMapper& rootIndices = d->engine->historyMatch.indices;
        if (idx.parent().isValid()) {
            if (idx.parent() != d->engine->curParent)
                return QModelIndex();
        } else {
            row = rootIndices.indexOf(idx.row());
            if (row == -1 && d->engine->curParent.isValid())
                return QModelIndex(); // source parent and our parent don't match
        }

        if (row == -1) {
            QIndexMapper& indices = d->engine->curMatch.indices;
            d->engine->filterOnDemand(idx.row() - indices.last());
            row = indices.indexOf(idx.row()) + rootIndices.count();
        }

        if (row == -1)
            return QModelIndex();
    } else {
        if (idx.parent() != d->engine->curParent)
            return QModelIndex();
        row = idx.row();
    }

    return createIndex(row, idx.column());
}

bool UiCompletionModel::setCurrentRow(int row)
{
    Q_D(UiCompletionModel);
    if (row < 0 || !d->engine->matchCount())
        return false;

    if (row >= d->engine->matchCount())
        d->engine->filterOnDemand(row + 1 - d->engine->matchCount());

    if (row >= d->engine->matchCount()) // invalid row
        return false;

    d->engine->curRow = row;
    return true;
}

QModelIndex UiCompletionModel::currentIndex(bool sourceIndex) const
{
    Q_D(const UiCompletionModel);

    if (!d->engine->matchCount())
        return QModelIndex();

    int row = d->engine->curRow;
    if (d->showAll)
        row = d->engine->curMatch.indices[d->engine->curRow];

    QModelIndex idx = createIndex(row, d->column);
    if (!sourceIndex)
        return idx;
    return mapToSource(idx);
}

QModelIndex UiCompletionModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_D(const UiCompletionModel);
    if (row < 0 || column < 0 || column >= columnCount(parent) || parent.isValid())
        return QModelIndex();

    if (!d->showAll) {
        if (!d->engine->matchCount())
            return QModelIndex();
        if (row >= d->engine->historyMatch.indices.count()) {
            int want = row + 1 - d->engine->matchCount();
            if (want > 0)
                d->engine->filterOnDemand(want);
            if (row >= d->engine->matchCount())
                return QModelIndex();
        }
    } else {
        if (row >= d->model->rowCount(d->engine->curParent))
            return QModelIndex();
    }

    return createIndex(row, column);
}

int UiCompletionModel::completionCount() const
{
    Q_D(const UiCompletionModel);
    if (!d->engine->matchCount())
        return 0;

    d->engine->filterOnDemand(INT_MAX);
    return d->engine->matchCount();
}

int UiCompletionModel::currentRow() const
{
    Q_D(const UiCompletionModel);
    return d->engine->curRow;
}

int UiCompletionModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const UiCompletionModel);
    if (parent.isValid())
        return 0;

    if (d->showAll) {
        // Show all items below current parent, even if we have no valid matches
        if (d->engine->curParts.count() != 1  && !d->engine->matchCount()
            && !d->engine->curParent.isValid())
            return 0;
        return d->model->rowCount(d->engine->curParent);
    }

    return completionCount();
}

void UiCompletionModel::setFiltered(bool filtered)
{
    Q_D(UiCompletionModel);
    if (d->showAll == !filtered)
        return;
    d->showAll = !filtered;
    resetModel();
}

bool UiCompletionModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const UiCompletionModel);
    if (parent.isValid())
        return false;

    if (d->showAll)
        return d->model->hasChildren(mapToSource(parent));

    if (!d->engine->matchCount())
        return false;

    return true;
}

QVariant UiCompletionModel::data(const QModelIndex& index, int role) const
{
    Q_D(const UiCompletionModel);
    return d->model->data(mapToSource(index), role);
}

void UiCompletionModel::modelDestroyed()
{
    QAbstractProxyModel::setSourceModel(0); // switch to static empty model
    invalidate();
}

void UiCompletionModel::rowsInserted()
{
    invalidate();
    emit rowsAdded();
}

void UiCompletionModel::invalidate()
{
    Q_D(UiCompletionModel);
    d->engine->cache.clear();
    filter(d->engine->curParts);
}

void UiCompletionModel::filter(const QStringList& parts)
{
    Q_D(UiCompletionModel);
    d->engine->filter(parts);
    resetModel();

    if (d->model->canFetchMore(d->engine->curParent))
        d->model->fetchMore(d->engine->curParent);
}

void UiCompletionModel::resetModel()
{
    beginResetModel();

    if (rowCount() > 0) {
        emit layoutAboutToBeChanged();
        QModelIndexList piList = persistentIndexList();
        QModelIndexList empty;
        for (int i = 0; i < piList.size(); i++)
            empty.append(QModelIndex());
        changePersistentIndexList(piList, empty);
        emit layoutChanged();
    }

    endResetModel();
}

/*!
    \property UiCompletionModel::modelSorting
    \brief the way the model is sorted

    By default, no assumptions are made about the order of the items
    in the model that provides the completions.

    If the model's data for the completionColumn() and completionRole() is sorted in
    ascending order, you can set this property to \l CaseSensitivelySortedModel
    or \l CaseInsensitivelySortedModel. On large models, this can lead to
    significant performance improvements because the completer object can
    then use a binary search algorithm instead of linear search algorithm.

    The sort order (i.e ascending or descending order) of the model is determined
    dynamically by inspecting the contents of the model.

    \b{Note:} The performance improvements described above cannot take place
    when the completer's \l caseSensitivity is different to the case sensitivity
    used by the model's when sorting.

    \sa setCaseSensitivity(), UiCompletionModel::ModelSorting
*/
void UiCompletionModel::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    Q_D(UiCompletionModel);
    if (d->cs == cs)
        return;
    d->cs = cs;
    createEngine();
    invalidate();
    emit caseSensitivityChanged();
}

Qt::CaseSensitivity UiCompletionModel::caseSensitivity() const
{
    Q_D(const UiCompletionModel);
    return d->cs;
}

/*!
    \property UiCompletionModel::modelSorting
    \brief the way the model is sorted

    By default, no assumptions are made about the order of the items
    in the model that provides the completions.

    If the model's data for the completionColumn() and completionRole() is sorted in
    ascending order, you can set this property to \l CaseSensitivelySortedModel
    or \l CaseInsensitivelySortedModel. On large models, this can lead to
    significant performance improvements because the completer object can
    then use a binary search algorithm instead of linear search algorithm.

    The sort order (i.e ascending or descending order) of the model is determined
    dynamically by inspecting the contents of the model.

    \b{Note:} The performance improvements described above cannot take place
    when the completer's \l caseSensitivity is different to the case sensitivity
    used by the model's when sorting.

    \sa setCaseSensitivity(), UiCompletionModel::ModelSorting
*/
void UiCompletionModel::setModelSorting(ModelSorting sorting)
{
    Q_D(UiCompletionModel);
    if (d->sorting == sorting)
        return;
    d->sorting = sorting;
    createEngine();
    invalidate();
    emit modelSortingChanged();
}

UiCompletionModel::ModelSorting UiCompletionModel::modelSorting() const
{
    Q_D(const UiCompletionModel);
    return d->sorting;
}

/*!
    \property UiCompletionModel::completionColumn
    \brief the column in the model in which completions are searched for.

    By default, the match column is 0.

    \sa completionRole, caseSensitivity
*/
void UiCompletionModel::setCompletionColumn(int column)
{
    Q_D(UiCompletionModel);
    if (d->column == column)
        return;
    d->column = column;
    invalidate();
    emit completionColumnChanged();
}

int UiCompletionModel::completionColumn() const
{
    Q_D(const UiCompletionModel);
    return d->column;
}

/*!
    \property UiCompletionModel::completionRole
    \brief the item role to be used to query the contents of items for matching.

    The default role is Qt::EditRole.

    \sa completionColumn, caseSensitivity
*/
void UiCompletionModel::setCompletionRole(int role)
{
    Q_D(UiCompletionModel);
    if (d->role == role)
        return;
    d->role = role;
    invalidate();
    emit completionRoleChanged();
}

int UiCompletionModel::completionRole() const
{
    Q_D(const UiCompletionModel);
    return d->role;
}

/*!
    \property UiCompletionModel::completionPrefix
    \brief the completion prefix used to provide completions.

    The completionModel() is updated to reflect the list of possible
    matches for \a prefix.
*/
void UiCompletionModel::setCompletionPrefix(const QString &prefix)
{
    Q_D(UiCompletionModel);
    d->prefix = prefix;
    filter(QStringList(prefix));
    emit completionPrefixChanged();
}

QString UiCompletionModel::completionPrefix() const
{
    Q_D(const UiCompletionModel);
    return d->prefix;
}

//////////////////////////////////////////////////////////////////////////////
void UiCompletionEngine::filter(const QStringList& parts)
{
    const QAbstractItemModel *model = c->proxy->sourceModel();
    curParts = parts;
    if (curParts.isEmpty())
        curParts.append(QString());

    curRow = -1;
    curParent = QModelIndex();
    curMatch = QMatchData();
    historyMatch = filterHistory();

    if (!model)
        return;

    QModelIndex parent;
    for (int i = 0; i < curParts.count() - 1; i++) {
        QString part = curParts[i];
        int emi = filter(part, parent, -1).exactMatchIndex;
        if (emi == -1)
            return;
        parent = model->index(emi, c->column, parent);
    }

    // Note that we set the curParent to a valid parent, even if we have no matches
    // When filtering is disabled, we show all the items under this parent
    curParent = parent;
    if (curParts.last().isEmpty())
        curMatch = QMatchData(QIndexMapper(0, model->rowCount(curParent) - 1), -1, false);
    else
        curMatch = filter(curParts.last(), curParent, 1); // build at least one
    curRow = curMatch.isValid() ? 0 : -1;
}

QMatchData UiCompletionEngine::filterHistory()
{
    QAbstractItemModel *source = c->proxy->sourceModel();
    if (curParts.count() <= 1 || c->showAll || !source)
        return QMatchData();

    QVector<int> v;
    QIndexMapper im(v);
    QMatchData m(im, -1, true);

    for (int i = 0; i < source->rowCount(); i++) {
        QString str = source->index(i, c->column).data().toString();
        if (str.startsWith(c->prefix, c->cs))
            m.indices.append(i);
    }
    return m;
}

// Returns a match hint from the cache by chopping the search string
bool UiCompletionEngine::matchHint(QString part, const QModelIndex& parent, QMatchData *hint)
{
    if (c->cs == Qt::CaseInsensitive)
        part = part.toLower();

    const CacheItem& map = cache[parent];

    QString key = part;
    while (!key.isEmpty()) {
        key.chop(1);
        if (map.contains(key)) {
            *hint = map[key];
            return true;
        }
    }

    return false;
}

bool UiCompletionEngine::lookupCache(QString part, const QModelIndex& parent, QMatchData *m)
{
   if (c->cs == Qt::CaseInsensitive)
        part = part.toLower();
   const CacheItem& map = cache[parent];
   if (!map.contains(part))
       return false;
   *m = map[part];
   return true;
}

// When the cache size exceeds 1MB, it clears out about 1/2 of the cache.
void UiCompletionEngine::saveInCache(QString part, const QModelIndex& parent, const QMatchData& m)
{
    QMatchData old = cache[parent].take(part);
    cost = cost + m.indices.cost() - old.indices.cost();
    if (cost * sizeof(int) > 1024 * 1024) {
        QMap<QModelIndex, CacheItem>::iterator it1 = cache.begin();
        while (it1 != cache.end()) {
            CacheItem& ci = it1.value();
            int sz = ci.count()/2;
            QMap<QString, QMatchData>::iterator it2 = ci.begin();
            int i = 0;
            while (it2 != ci.end() && i < sz) {
                cost -= it2.value().indices.cost();
                it2 = ci.erase(it2);
                i++;
            }
            if (ci.count() == 0) {
              it1 = cache.erase(it1);
            } else {
              ++it1;
            }
        }
    }

    if (c->cs == Qt::CaseInsensitive)
        part = part.toLower();
    cache[parent][part] = m;
}

///////////////////////////////////////////////////////////////////////////////////
QIndexMapper QSortedModelEngine::indexHint(QString part, const QModelIndex& parent, Qt::SortOrder order)
{
    const QAbstractItemModel *model = c->proxy->sourceModel();

    if (c->cs == Qt::CaseInsensitive)
        part = part.toLower();

    const CacheItem& map = cache[parent];

    // Try to find a lower and upper bound for the search from previous results
    int to = model->rowCount(parent) - 1;
    int from = 0;
    const CacheItem::const_iterator it = map.lowerBound(part);

    // look backward for first valid hint
    for (CacheItem::const_iterator it1 = it; it1-- != map.constBegin();) {
        const QMatchData& value = it1.value();
        if (value.isValid()) {
            if (order == Qt::AscendingOrder) {
                from = value.indices.last() + 1;
            } else {
                to = value.indices.first() - 1;
            }
            break;
        }
    }

    // look forward for first valid hint
    for (CacheItem::const_iterator it2 = it; it2 != map.constEnd(); ++it2) {
        const QMatchData& value = it2.value();
        if (value.isValid() && !it2.key().startsWith(part)) {
            if (order == Qt::AscendingOrder) {
                to = value.indices.first() - 1;
            } else {
                from = value.indices.first() + 1;
            }
            break;
        }
    }

    return QIndexMapper(from, to);
}

Qt::SortOrder QSortedModelEngine::sortOrder(const QModelIndex &parent) const
{
    const QAbstractItemModel *model = c->proxy->sourceModel();

    int rowCount = model->rowCount(parent);
    if (rowCount < 2)
        return Qt::AscendingOrder;
    QString first = model->data(model->index(0, c->column, parent), c->role).toString();
    QString last = model->data(model->index(rowCount - 1, c->column, parent), c->role).toString();
    return QString::compare(first, last, c->cs) <= 0 ? Qt::AscendingOrder : Qt::DescendingOrder;
}

QMatchData QSortedModelEngine::filter(const QString& part, const QModelIndex& parent, int)
{
    const QAbstractItemModel *model = c->proxy->sourceModel();

    QMatchData hint;
    if (lookupCache(part, parent, &hint))
        return hint;

    QIndexMapper indices;
    Qt::SortOrder order = sortOrder(parent);

    if (matchHint(part, parent, &hint)) {
        if (!hint.isValid())
            return QMatchData();
        indices = hint.indices;
    } else {
        indices = indexHint(part, parent, order);
    }

    // binary search the model within 'indices' for 'part' under 'parent'
    int high = indices.to() + 1;
    int low = indices.from() - 1;
    int probe;
    QModelIndex probeIndex;
    QString probeData;

    while (high - low > 1)
    {
        probe = (high + low) / 2;
        probeIndex = model->index(probe, c->column, parent);
        probeData = model->data(probeIndex, c->role).toString();
        const int cmp = QString::compare(probeData, part, c->cs);
        if ((order == Qt::AscendingOrder && cmp >= 0)
            || (order == Qt::DescendingOrder && cmp < 0)) {
            high = probe;
        } else {
            low = probe;
        }
    }

    if ((order == Qt::AscendingOrder && low == indices.to())
        || (order == Qt::DescendingOrder && high == indices.from())) { // not found
        saveInCache(part, parent, QMatchData());
        return QMatchData();
    }

    probeIndex = model->index(order == Qt::AscendingOrder ? low+1 : high-1, c->column, parent);
    probeData = model->data(probeIndex, c->role).toString();
    if (!probeData.startsWith(part, c->cs)) {
        saveInCache(part, parent, QMatchData());
        return QMatchData();
    }

    const bool exactMatch = QString::compare(probeData, part, c->cs) == 0;
    int emi =  exactMatch ? (order == Qt::AscendingOrder ? low+1 : high-1) : -1;

    int from = 0;
    int to = 0;
    if (order == Qt::AscendingOrder) {
        from = low + 1;
        high = indices.to() + 1;
        low = from;
    } else {
        to = high - 1;
        low = indices.from() - 1;
        high = to;
    }

    while (high - low > 1)
    {
        probe = (high + low) / 2;
        probeIndex = model->index(probe, c->column, parent);
        probeData = model->data(probeIndex, c->role).toString();
        const bool startsWith = probeData.startsWith(part, c->cs);
        if ((order == Qt::AscendingOrder && startsWith)
            || (order == Qt::DescendingOrder && !startsWith)) {
            low = probe;
        } else {
            high = probe;
        }
    }

    QMatchData m(order == Qt::AscendingOrder ? QIndexMapper(from, high - 1) : QIndexMapper(low+1, to), emi, false);
    saveInCache(part, parent, m);
    return m;
}

////////////////////////////////////////////////////////////////////////////////////////
int QUnsortedModelEngine::buildIndices(const QString& str, const QModelIndex& parent, int n,
                                      const QIndexMapper& indices, QMatchData* m)
{
    Q_ASSERT(m->partial);
    Q_ASSERT(n != -1 || m->exactMatchIndex == -1);
    const QAbstractItemModel *model = c->proxy->sourceModel();
    int i, count = 0;

    for (i = 0; i < indices.count() && count != n; ++i) {
        QModelIndex idx = model->index(indices[i], c->column, parent);
        QString data = model->data(idx, c->role).toString();
        if (!data.startsWith(str, c->cs) || !(model->flags(idx) & Qt::ItemIsSelectable))
            continue;
        m->indices.append(indices[i]);
        ++count;
        if (m->exactMatchIndex == -1 && QString::compare(data, str, c->cs) == 0) {
            m->exactMatchIndex = indices[i];
            if (n == -1)
                return indices[i];
        }
    }
    return indices[i-1];
}

void QUnsortedModelEngine::filterOnDemand(int n)
{
    Q_ASSERT(matchCount());
    if (!curMatch.partial)
        return;
    Q_ASSERT(n >= -1);
    const QAbstractItemModel *model = c->proxy->sourceModel();
    int lastRow = model->rowCount(curParent) - 1;
    QIndexMapper im(curMatch.indices.last() + 1, lastRow);
    int lastIndex = buildIndices(curParts.last(), curParent, n, im, &curMatch);
    curMatch.partial = (lastRow != lastIndex);
    saveInCache(curParts.last(), curParent, curMatch);
}

QMatchData QUnsortedModelEngine::filter(const QString& part, const QModelIndex& parent, int n)
{
    QMatchData hint;

    QVector<int> v;
    QIndexMapper im(v);
    QMatchData m(im, -1, true);

    const QAbstractItemModel *model = c->proxy->sourceModel();
    bool foundInCache = lookupCache(part, parent, &m);

    if (!foundInCache) {
        if (matchHint(part, parent, &hint) && !hint.isValid())
            return QMatchData();
    }

    if (!foundInCache && !hint.isValid()) {
        const int lastRow = model->rowCount(parent) - 1;
        QIndexMapper all(0, lastRow);
        int lastIndex = buildIndices(part, parent, n, all, &m);
        m.partial = (lastIndex != lastRow);
    } else {
        if (!foundInCache) { // build from hint as much as we can
            buildIndices(part, parent, INT_MAX, hint.indices, &m);
            m.partial = hint.partial;
        }
        if (m.partial && ((n == -1 && m.exactMatchIndex == -1) || (m.indices.count() < n))) {
            // need more and have more
            const int lastRow = model->rowCount(parent) - 1;
            QIndexMapper rest(hint.indices.last() + 1, lastRow);
            int want = n == -1 ? -1 : n - m.indices.count();
            int lastIndex = buildIndices(part, parent, want, rest, &m);
            m.partial = (lastRow != lastIndex);
        }
    }

    saveInCache(part, parent, m);
    return m;
}

///////////////////////////////////////////////////////////////////////////////

QT_END_NAMESPACE_UIHELPERS

#include "moc_uicompletionmodel.cpp"

#endif // QT_NO_COMPLETIONMODEL
