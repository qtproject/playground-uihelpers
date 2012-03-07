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
//#include <QtGui/qbrush.h>
//#include <QtGui/qfont.h>
//#include <QtWidgets/qicon.h>
#ifndef QT_NO_DATASTREAM
#include <QtCore/qdatastream.h>
#include <QtCore/qlist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_UIHELPERS

#ifndef QT_NO_STANDARDITEMMODEL

//template <class T> class QList;

class UiStandardItemModel;

class UiStandardItemPrivate;
class UIHELPERS_EXPORT UiStandardItem
{
public:
    UiStandardItem();
    UiStandardItem(const QString &text);
//    UiStandardItem(const QIcon &icon, const QString &text);
    explicit UiStandardItem(int rows, int columns = 1);
    virtual ~UiStandardItem();

    virtual QVariant data(int role = Qt::UserRole + 1) const;
    virtual void setData(const QVariant &value, int role = Qt::UserRole + 1);

    inline QString text() const {
        return qvariant_cast<QString>(data(Qt::DisplayRole));
    }
    inline void setText(const QString &text);

//    inline QIcon icon() const {
//        return qvariant_cast<QIcon>(data(Qt::DecorationRole));
//    }
//    inline void setIcon(const QIcon &icon);

//#ifndef QT_NO_TOOLTIP
//    inline QString toolTip() const {
//        return qvariant_cast<QString>(data(Qt::ToolTipRole));
//    }
//    inline void setToolTip(const QString &toolTip);
//#endif

//#ifndef QT_NO_STATUSTIP
//    inline QString statusTip() const {
//        return qvariant_cast<QString>(data(Qt::StatusTipRole));
//    }
//    inline void setStatusTip(const QString &statusTip);
//#endif
//
//#ifndef QT_NO_WHATSTHIS
//    inline QString whatsThis() const {
//        return qvariant_cast<QString>(data(Qt::WhatsThisRole));
//    }
//    inline void setWhatsThis(const QString &whatsThis);
//#endif
//
//    inline QSize sizeHint() const {
//        return qvariant_cast<QSize>(data(Qt::SizeHintRole));
//    }
//    inline void setSizeHint(const QSize &sizeHint);
//
//    inline QFont font() const {
//        return qvariant_cast<QFont>(data(Qt::FontRole));
//    }
//    inline void setFont(const QFont &font);
//
//    inline Qt::Alignment textAlignment() const {
//        return Qt::Alignment(qvariant_cast<int>(data(Qt::TextAlignmentRole)));
//    }
//    inline void setTextAlignment(Qt::Alignment textAlignment);
//
//    inline QBrush background() const {
//        return qvariant_cast<QBrush>(data(Qt::BackgroundRole));
//    }
//    inline void setBackground(const QBrush &brush);
//
//    inline QBrush foreground() const {
//        return qvariant_cast<QBrush>(data(Qt::ForegroundRole));
//    }
//    inline void setForeground(const QBrush &brush);
//
//    inline Qt::CheckState checkState() const {
//        return Qt::CheckState(qvariant_cast<int>(data(Qt::CheckStateRole)));
//    }
//    inline void setCheckState(Qt::CheckState checkState);
//
//    inline QString accessibleText() const {
//        return qvariant_cast<QString>(data(Qt::AccessibleTextRole));
//    }
//    inline void setAccessibleText(const QString &accessibleText);
//
//    inline QString accessibleDescription() const {
//        return qvariant_cast<QString>(data(Qt::AccessibleDescriptionRole));
//    }
//    inline void setAccessibleDescription(const QString &accessibleDescription);

    Qt::ItemFlags flags() const;
    void setFlags(Qt::ItemFlags flags);
//
//    inline bool isEnabled() const {
//        return (flags() & Qt::ItemIsEnabled) != 0;
//    }
//    void setEnabled(bool enabled);
//
//    inline bool isEditable() const {
//        return (flags() & Qt::ItemIsEditable) != 0;
//    }
//    void setEditable(bool editable);
//
//    inline bool isSelectable() const {
//        return (flags() & Qt::ItemIsSelectable) != 0;
//    }
//    void setSelectable(bool selectable);
//
//    inline bool isCheckable() const {
//        return (flags() & Qt::ItemIsUserCheckable) != 0;
//    }
//    void setCheckable(bool checkable);
//
//    inline bool isTristate() const {
//        return (flags() & Qt::ItemIsTristate) != 0;
//    }
//    void setTristate(bool tristate);
//
//#ifndef QT_NO_DRAGANDDROP
//    inline bool isDragEnabled() const {
//        return (flags() & Qt::ItemIsDragEnabled) != 0;
//    }
//    void setDragEnabled(bool dragEnabled);
//
//    inline bool isDropEnabled() const {
//        return (flags() & Qt::ItemIsDropEnabled) != 0;
//    }
//    void setDropEnabled(bool dropEnabled);
//#endif // QT_NO_DRAGANDDROP

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

//inline void UiStandardItem::setIcon(const QIcon &aicon)
//{ setData(aicon, Qt::DecorationRole); }
//
//#ifndef QT_NO_TOOLTIP
//inline void UiStandardItem::setToolTip(const QString &atoolTip)
//{ setData(atoolTip, Qt::ToolTipRole); }
//#endif
//
//#ifndef QT_NO_STATUSTIP
//inline void UiStandardItem::setStatusTip(const QString &astatusTip)
//{ setData(astatusTip, Qt::StatusTipRole); }
//#endif
//
//#ifndef QT_NO_WHATSTHIS
//inline void UiStandardItem::setWhatsThis(const QString &awhatsThis)
//{ setData(awhatsThis, Qt::WhatsThisRole); }
//#endif
//
//inline void UiStandardItem::setSizeHint(const QSize &asizeHint)
//{ setData(asizeHint, Qt::SizeHintRole); }
//
//inline void UiStandardItem::setFont(const QFont &afont)
//{ setData(afont, Qt::FontRole); }
//
//inline void UiStandardItem::setTextAlignment(Qt::Alignment atextAlignment)
//{ setData(int(atextAlignment), Qt::TextAlignmentRole); }
//
//inline void UiStandardItem::setBackground(const QBrush &abrush)
//{ setData(abrush, Qt::BackgroundRole); }
//
//inline void UiStandardItem::setForeground(const QBrush &abrush)
//{ setData(abrush, Qt::ForegroundRole); }
//
//inline void UiStandardItem::setCheckState(Qt::CheckState acheckState)
//{ setData(acheckState, Qt::CheckStateRole); }
//
//inline void UiStandardItem::setAccessibleText(const QString &aaccessibleText)
//{ setData(aaccessibleText, Qt::AccessibleTextRole); }
//
//inline void UiStandardItem::setAccessibleDescription(const QString &aaccessibleDescription)
//{ setData(aaccessibleDescription, Qt::AccessibleDescriptionRole); }

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

//    QVariant headerData(int section, Qt::Orientation orientation,
//                        int role = Qt::DisplayRole) const;
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
//                       int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    // Qt::DropActions supportedDropActions() const;

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

//    UiStandardItem *horizontalHeaderItem(int column) const;
//    void setHorizontalHeaderItem(int column, UiStandardItem *item);
//    UiStandardItem *verticalHeaderItem(int row) const;
//    void setVerticalHeaderItem(int row, UiStandardItem *item);
//
//    void setHorizontalHeaderLabels(const QStringList &labels);
//    void setVerticalHeaderLabels(const QStringList &labels);

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

//    UiStandardItem *takeHorizontalHeaderItem(int column);
//    UiStandardItem *takeVerticalHeaderItem(int row);

    const UiStandardItem *itemPrototype() const;
    void setItemPrototype(const UiStandardItem *item);

    QList<UiStandardItem*> findItems(const QString &text,
                                    Qt::MatchFlags flags = Qt::MatchExactly,
                                    int column = 0) const;

    int sortRole() const;
    void setSortRole(int role);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    // bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

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
