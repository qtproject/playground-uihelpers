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

#ifndef UISTANDARDITEMMODEL_P_H
#define UISTANDARDITEMMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qabstractitemmodel_p.h>

#ifndef QT_NO_STANDARDITEMMODEL

#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>
#include "uihelpersglobal.h"

QT_BEGIN_NAMESPACE_UIHELPERS

// from src/widgets/itemviews/qwidgetitemdata_p.h

class UiStandardItemData
{
public:
    inline UiStandardItemData() : role(-1) {}
    inline UiStandardItemData(int r, QVariant v) : role(r), value(v) {}
    int role;
    QVariant value;
    inline bool operator==(const UiStandardItemData &other) const { return role == other.role && value == other.value; }
};

#ifndef QT_NO_DATASTREAM

inline QDataStream &operator>>(QDataStream &in, UiStandardItemData &data)
{
    in >> data.role;
    in >> data.value;
    return in;
}

inline QDataStream &operator<<(QDataStream &out, const UiStandardItemData &data)
{
    out << data.role;
    out << data.value;
    return out;
}

#endif // QT_NO_DATASTREAM

//

class UiStandardItemPrivate
{
    Q_DECLARE_PUBLIC(UiStandardItem)
public:
    inline UiStandardItemPrivate()
        : model(0),
          parent(0),
          rows(0),
          columns(0),
          q_ptr(0),
          lastIndexOf(2)
        { }
    virtual ~UiStandardItemPrivate();

    inline int childIndex(int row, int column) const {
        if ((row < 0) || (column < 0)
            || (row >= rowCount()) || (column >= columnCount())) {
            return -1;
        }
        return (row * columnCount()) + column;
    }
    inline int childIndex(const UiStandardItem *child) {
        int start = qMax(0, lastIndexOf -2);
        lastIndexOf = children.indexOf(const_cast<UiStandardItem*>(child), start);
        if (lastIndexOf == -1 && start != 0)
            lastIndexOf = children.lastIndexOf(const_cast<UiStandardItem*>(child), start);
        return lastIndexOf;
    }
    QPair<int, int> position() const;
    void setChild(int row, int column, UiStandardItem *item,
                  bool emitChanged = false);
    inline int rowCount() const {
        return rows;
    }
    inline int columnCount() const {
        return columns;
    }
    void childDeleted(UiStandardItem *child);

    void setModel(UiStandardItemModel *mod);

    inline void setParentAndModel(
        UiStandardItem *par,
        UiStandardItemModel *mod) {
        setModel(mod);
        parent = par;
    }

    void changeFlags(bool enable, Qt::ItemFlags f);
    void setItemData(const QMap<int, QVariant> &roles);
    const QMap<int, QVariant> itemData() const;

    bool insertRows(int row, int count, const QList<UiStandardItem*> &items);
    bool insertRows(int row, const QList<UiStandardItem*> &items);
    bool insertColumns(int column, int count, const QList<UiStandardItem*> &items);

    void sortChildren(int column, Qt::SortOrder order);

    UiStandardItemModel *model;
    UiStandardItem *parent;
    QVector<UiStandardItemData> values;
    QVector<UiStandardItem*> children;
    int rows;
    int columns;

    UiStandardItem *q_ptr;

    int lastIndexOf;
};

class UiStandardItemModelPrivate : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(UiStandardItemModel)

public:
    UiStandardItemModelPrivate();
    virtual ~UiStandardItemModelPrivate();

    void init();

    inline UiStandardItem *createItem() const {
        return itemPrototype ? itemPrototype->clone() : new UiStandardItem;
    }

    inline UiStandardItem *itemFromIndex(const QModelIndex &index) const {
        Q_Q(const UiStandardItemModel);
        if (!index.isValid())
            return root.data();
        if (index.model() != q)
            return 0;
        UiStandardItem *parent = static_cast<UiStandardItem*>(index.internalPointer());
        if (parent == 0)
            return 0;
        return parent->child(index.row(), index.column());
    }

    void sort(UiStandardItem *parent, int column, Qt::SortOrder order);
    void itemChanged(UiStandardItem *item);
    void rowsAboutToBeInserted(UiStandardItem *parent, int start, int end);
    void columnsAboutToBeInserted(UiStandardItem *parent, int start, int end);
    void rowsAboutToBeRemoved(UiStandardItem *parent, int start, int end);
    void columnsAboutToBeRemoved(UiStandardItem *parent, int start, int end);
    void rowsInserted(UiStandardItem *parent, int row, int count);
    void columnsInserted(UiStandardItem *parent, int column, int count);
    void rowsRemoved(UiStandardItem *parent, int row, int count);
    void columnsRemoved(UiStandardItem *parent, int column, int count);

    void _q_emitItemChanged(const QModelIndex &topLeft,
                            const QModelIndex &bottomRight);

    // Used only by UiStandardItemModel::dropMimeData
    // void decodeDataRecursive(QDataStream &stream, UiStandardItem *item);

    // QVector<UiStandardItem*> columnHeaderItems;
    // QVector<UiStandardItem*> rowHeaderItems;
    QScopedPointer<UiStandardItem> root;
    const UiStandardItem *itemPrototype;
    int sortRole;
};

QT_END_NAMESPACE_UIHELPERS

#endif // QT_NO_STANDARDITEMMODEL

#endif // UISTANDARDITEMMODEL_P_H
