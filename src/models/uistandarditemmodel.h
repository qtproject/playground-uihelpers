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

#ifndef UISTANDARDITEMMODEL_H
#define UISTANDARDITEMMODEL_H

#include "uihelpersglobal.h"
#include <QtCore/qabstractitemmodel.h>
#ifndef QT_NO_DATASTREAM
#include <QtCore/qdatastream.h>
#include <QtCore/qlist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_UIHELPERS

#ifndef QT_NO_STANDARDITEMMODEL

class UiStandardItemModel;

class UiStandardItemPrivate;
class UIHELPERS_EXPORT UiStandardItem
{
public:
    UiStandardItem();
    UiStandardItem(const QString &text);
    explicit UiStandardItem(int rows, int columns = 1);
    virtual ~UiStandardItem();

    virtual QVariant data(int role = Qt::UserRole + 1) const;
    virtual void setData(const QVariant &value, int role = Qt::UserRole + 1);

    inline QString text() const {
        return qvariant_cast<QString>(data(Qt::DisplayRole));
    }
    inline void setText(const QString &text);

    Qt::ItemFlags flags() const;
    void setFlags(Qt::ItemFlags flags);

    inline bool isEnabled() const {
        return (flags() & Qt::ItemIsEnabled) != 0;
    }
    void setEnabled(bool enabled);

    inline bool isEditable() const {
        return (flags() & Qt::ItemIsEditable) != 0;
    }
    void setEditable(bool editable);

    UiStandardItem *parent() const;
    int row() const;
    int column() const;
    QModelIndex index() const;
    UiStandardItemModel *model() const;

    int rowCount() const;
    void setRowCount(int rows);
    int columnCount() const;
    void setColumnCount(int columns);

    bool hasChildren() const;
    UiStandardItem *child(int row, int column = 0) const;
    void setChild(int row, int column, UiStandardItem *item);
    inline void setChild(int row, UiStandardItem *item);

    void insertRow(int row, const QList<UiStandardItem*> &items);
    void insertColumn(int column, const QList<UiStandardItem*> &items);
    void insertRows(int row, const QList<UiStandardItem*> &items);
    void insertRows(int row, int count);
    void insertColumns(int column, int count);

    void removeRow(int row);
    void removeColumn(int column);
    void removeRows(int row, int count);
    void removeColumns(int column, int count);

    inline void appendRow(const QList<UiStandardItem*> &items);
    inline void appendRows(const QList<UiStandardItem*> &items);
    inline void appendColumn(const QList<UiStandardItem*> &items);
    inline void insertRow(int row, UiStandardItem *item);
    inline void appendRow(UiStandardItem *item);

    UiStandardItem *takeChild(int row, int column = 0);
    QList<UiStandardItem*> takeRow(int row);
    QList<UiStandardItem*> takeColumn(int column);

    void sortChildren(int column, Qt::SortOrder order = Qt::AscendingOrder);

    virtual UiStandardItem *clone() const;

    enum ItemType { Type = 0, UserType = 1000 };
    virtual int type() const;

#ifndef QT_NO_DATASTREAM
    virtual void read(QDataStream &in);
    virtual void write(QDataStream &out) const;
#endif
    virtual bool operator<(const UiStandardItem &other) const;

protected:
    UiStandardItem(const UiStandardItem &other);
    UiStandardItem(UiStandardItemPrivate &dd);
    UiStandardItem &operator=(const UiStandardItem &other);
    QScopedPointer<UiStandardItemPrivate> d_ptr;

    void emitDataChanged();

private:
    Q_DECLARE_PRIVATE(UiStandardItem)
    friend class UiStandardItemModelPrivate;
    friend class UiStandardItemModel;
};

inline void UiStandardItem::setText(const QString &atext)
{ setData(atext, Qt::DisplayRole); }

inline void UiStandardItem::setChild(int arow, UiStandardItem *aitem)
{ setChild(arow, 0, aitem); }

inline void UiStandardItem::appendRow(const QList<UiStandardItem*> &aitems)
{ insertRow(rowCount(), aitems); }

inline void UiStandardItem::appendRows(const QList<UiStandardItem*> &aitems)
{ insertRows(rowCount(), aitems); }

inline void UiStandardItem::appendColumn(const QList<UiStandardItem*> &aitems)
{ insertColumn(columnCount(), aitems); }

inline void UiStandardItem::insertRow(int arow, UiStandardItem *aitem)
{ insertRow(arow, QList<UiStandardItem*>() << aitem); }

inline void UiStandardItem::appendRow(UiStandardItem *aitem)
{ insertRow(rowCount(), aitem); }

class UiStandardItemModelPrivate;

class UIHELPERS_EXPORT UiStandardItemModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int sortRole READ sortRole WRITE setSortRole)

public:
    explicit UiStandardItemModel(QObject *parent = 0);
    UiStandardItemModel(int rows, int columns, QObject *parent = 0);
    ~UiStandardItemModel();

    void setItemRoleNames(const QHash<int,QByteArray> &roleNames);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

    void clear();

#ifdef Q_NO_USING_KEYWORD
    inline QObject *parent() const { return QObject::parent(); }
#else
    using QObject::parent;
#endif

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    UiStandardItem *itemFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromItem(const UiStandardItem *item) const;

    UiStandardItem *item(int row, int column = 0) const;
    void setItem(int row, int column, UiStandardItem *item);
    inline void setItem(int row, UiStandardItem *item);
    UiStandardItem *invisibleRootItem() const;

    void setRowCount(int rows);
    void setColumnCount(int columns);

    void appendRow(const QList<UiStandardItem*> &items);
    void appendColumn(const QList<UiStandardItem*> &items);
    inline void appendRow(UiStandardItem *item);

    void insertRow(int row, const QList<UiStandardItem*> &items);
    void insertColumn(int column, const QList<UiStandardItem*> &items);
    inline void insertRow(int row, UiStandardItem *item);

    inline bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    inline bool insertColumn(int column, const QModelIndex &parent = QModelIndex());

    UiStandardItem *takeItem(int row, int column = 0);
    QList<UiStandardItem*> takeRow(int row);
    QList<UiStandardItem*> takeColumn(int column);

    const UiStandardItem *itemPrototype() const;
    void setItemPrototype(const UiStandardItem *item);

    QList<UiStandardItem*> findItems(const QString &text,
                                    Qt::MatchFlags flags = Qt::MatchExactly,
                                    int column = 0) const;

    int sortRole() const;
    void setSortRole(int role);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

Q_SIGNALS:
    void itemChanged(UiStandardItem *item);

protected:
    UiStandardItemModel(UiStandardItemModelPrivate &dd, QObject *parent = 0);

private:
    friend class UiStandardItemPrivate;
    friend class UiStandardItem;
    Q_DISABLE_COPY(UiStandardItemModel)
    Q_DECLARE_PRIVATE(UiStandardItemModel)

    Q_PRIVATE_SLOT(d_func(), void _q_emitItemChanged(const QModelIndex &topLeft,
                                                     const QModelIndex &bottomRight))
};

inline void UiStandardItemModel::setItem(int arow, UiStandardItem *aitem)
{ setItem(arow, 0, aitem); }

inline void UiStandardItemModel::appendRow(UiStandardItem *aitem)
{ appendRow(QList<UiStandardItem*>() << aitem); }

inline void UiStandardItemModel::insertRow(int arow, UiStandardItem *aitem)
{ insertRow(arow, QList<UiStandardItem*>() << aitem); }

inline bool UiStandardItemModel::insertRow(int arow, const QModelIndex &aparent)
{ return QAbstractItemModel::insertRow(arow, aparent); }
inline bool UiStandardItemModel::insertColumn(int acolumn, const QModelIndex &aparent)
{ return QAbstractItemModel::insertColumn(acolumn, aparent); }

#ifndef QT_NO_DATASTREAM
UIHELPERS_EXPORT QDataStream &operator>>(QDataStream &in, UiStandardItem &item);
UIHELPERS_EXPORT QDataStream &operator<<(QDataStream &out, const UiStandardItem &item);
#endif

#endif // QT_NO_STANDARDITEMMODEL

QT_END_NAMESPACE_UIHELPERS

QT_END_HEADER

#endif //UISTANDARDITEMMODEL_H
