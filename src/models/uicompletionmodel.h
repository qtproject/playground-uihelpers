/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT)
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

#ifndef UICOMPLETIONMODEL_H
#define UICOMPLETIONMODEL_H

#ifndef QT_NO_COMPLETIONMODEL

#include "uihelpersglobal.h"
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstring.h>
#include <QtCore/qabstractitemmodel.h>
#include "QtCore/qabstractproxymodel.h"
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_UIHELPERS

class UiCompletionModelPrivate;

class UIHELPERS_EXPORT UiCompletionModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_ENUMS(ModelSorting)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity)
    Q_PROPERTY(ModelSorting modelSorting READ modelSorting WRITE setModelSorting)
    Q_PROPERTY(int completionColumn READ completionColumn WRITE setCompletionColumn)
    Q_PROPERTY(int completionRole READ completionRole WRITE setCompletionRole)
    Q_PROPERTY(QString completionPrefix READ completionPrefix WRITE setCompletionPrefix)

public:
    enum ModelSorting {
        UnsortedModel = 0,
        CaseSensitivelySortedModel,
        CaseInsensitivelySortedModel
    };

    UiCompletionModel(QObject *parent = 0);

    void setFiltered(bool);
    void filter(const QStringList& parts);
    int completionCount() const;
    int currentRow() const;
    bool setCurrentRow(int row);
    QModelIndex currentIndex(bool) const;
    void resetModel();

    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
    Qt::CaseSensitivity caseSensitivity() const;
    void setModelSorting(ModelSorting sorting);
    ModelSorting modelSorting() const;
    void setCompletionColumn(int column);
    int completionColumn() const;
    void setCompletionRole(int role);
    int completionRole() const;
    QString completionPrefix() const;

    QModelIndex index(int row, int column, const QModelIndex & = QModelIndex()) const;
    int rowCount(const QModelIndex &index = QModelIndex()) const;
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & = QModelIndex()) const { return QModelIndex(); }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    void setSourceModel(QAbstractItemModel *sourceModel);
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;

    Q_DECLARE_PRIVATE(UiCompletionModel)

protected:
    void createEngine();

signals:
    void rowsAdded();

public Q_SLOTS:
    void invalidate();
    void rowsInserted();
    void modelDestroyed();
    void setCompletionPrefix(const QString &prefix);
};

QT_END_NAMESPACE_UIHELPERS

QT_END_HEADER

#endif // QT_NO_COMPLETIONMODEL
#endif // UICOMPLETIONMODEL_H
